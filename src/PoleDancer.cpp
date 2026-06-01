#include <cmath>
#include "plugin.hpp"
#include "modulehelpers.hpp"
#include "zcomponentlib.hpp"
#include "TurnsCountingKnob.hpp"

namespace noxious {

/* code below is copyright 2016-2023 VCV under the GPL version 3.0 or later */

struct PoleMixCoefficients {
  float weight[5] = {};
};


using simd::float_4;

template <typename T>
static T clip(T x) {
  // return std::tanh(x);
  // Pade approximant of tanh
  x = simd::clamp(x, -3.f, 3.f);
  return x * (27 + x * x) / (27 + 9 * x * x);
}


template <typename T>
struct LadderFilter {
  T omega0;
  T resonance = 1;
  T state[4];
  T input;

  LadderFilter() {
    reset();
    setCutoff(0);
  }

  void reset() {
    for (int i = 0; i < 4; i++) {
      state[i] = 0;
    }
  }

  void setCutoff(T cutoff) {
    omega0 = 2 * T(M_PI) * cutoff;
  }

  void process(T input, T dt) {
    dsp::stepRK4(T(0), dt, state, 4, [&](T t, const T x[], T dxdt[]) {
        T inputt = crossfade(this->input, input, t / dt);
        T inputc = clip(inputt - resonance * x[3]);
        T yc0 = clip(x[0]);
        T yc1 = clip(x[1]);
        T yc2 = clip(x[2]);
        T yc3 = clip(x[3]);

        dxdt[0] = omega0 * (inputc - yc0);
        dxdt[1] = omega0 * (yc0 - yc1);
        dxdt[2] = omega0 * (yc1 - yc2);
        dxdt[3] = omega0 * (yc2 - yc3);
      });

    this->input = input;
  }

  T lowpass() {
    return state[3];
  }
  T highpass() {
    return clip((input - resonance * state[3]) - 4 * state[0] + 6 * state[1] - 4 * state[2] + state[3]);
  }
  T poleMix(PoleMixCoefficients poleMix) {
    // scale by the weights, then return them to full scale
    T mix = 
      poleMix.weight[0] * (input - resonance * state[3]) -
      poleMix.weight[1] * state[0] +
      poleMix.weight[2] * state[1] -
      poleMix.weight[3] * state[2] +
      poleMix.weight[4] * state[3];
    return clip((1/POLEMIX_VOLTAGE_SCALE) * mix);
  }
};



struct PoleDancer : Module {
  enum ParamIds {
    CUTOFF_PARAM,
    RES_KNOB_PARAM,
    CUTOFF_CV_PARAM,
    RES_CV_PARAM,
    NUM_PARAMS
  };
  enum InputIds {
    CUTOFF_INPUT,
    RESONANCE_INPUT,
    IN_INPUT,
    POLE_MIX_INPUT,
    NUM_INPUTS
  };
  enum OutputIds {
    MIX_OUTPUT,
    NUM_OUTPUTS
  };

  LadderFilter<float_4> filters[4];
  PoleMixCoefficients poleMix;
  PersonalityMessage expanderMessages[2] = {};
  dsp::ClockDivider expanderClockDivider;

  PoleDancer() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
    configParam(CUTOFF_PARAM, -5.f, 5.f, 0.f, "Cutoff frequency", " Hz", 2.f, dsp::FREQ_C4);
    configParam(RES_KNOB_PARAM, 0.f, 1.f, 0.f, "Resonance", "%", 0.f, 100.f);
    configParam(RES_CV_PARAM, -1.f, 1.f, 0.f, "Resonance CV", "%", 0.f, 100.f);
    configParam(CUTOFF_CV_PARAM, -1.f, 1.f, 0.f, "Cutoff frequency CV", "%", 0.f, 100.f);
    configInput(CUTOFF_INPUT, "Cutoff CV In");
    configInput(RESONANCE_INPUT, "Resonance CV In");
    configInput(IN_INPUT, "Audio Input");
    configInput(POLE_MIX_INPUT, "Pole Mix(5)");
    configOutput(MIX_OUTPUT, "Filter Output");

    configBypass(IN_INPUT, MIX_OUTPUT);

    // expanders are UI elements, not audio rate
    rightExpander.producerMessage = &expanderMessages[0];
    rightExpander.consumerMessage = &expanderMessages[1];
    expanderClockDivider.setDivision(EXPANDER_CLOCK_DIV);
  }

  void onReset() override {
    for (int i = 0; i < 4; i++) {
      filters[i].reset();
    }
  }

  void process(const ProcessArgs& args) override {
    bool isExpanderClockTick = expanderClockDivider.process();

    if (inputs[POLE_MIX_INPUT].isConnected()) {
      int n = inputs[POLE_MIX_INPUT].getChannels();

      for (int i = 0; i < 5; ++i) {
        //poleMix.weight[i] = (n > i) ? poleMixInputToCoeff(inputs[POLE_MIX_INPUT].getVoltage(i)) : 0.f;
        poleMix.weight[i] = (n > i) ? inputs[POLE_MIX_INPUT].getVoltage(i) : 0.f;
      }
    }
    else {
      // 4-pole lowpass if nothing connected
      poleMix.weight[0] = 0.f;
      poleMix.weight[1] = 0.f;
      poleMix.weight[2] = 0.f;
      poleMix.weight[3] = 0.f;
      poleMix.weight[4] = POLEMIX_VOLTAGE_SCALE;
    }

    float resParam = params[RES_KNOB_PARAM].getValue();
    float resCvParam = params[RES_CV_PARAM].getValue();
    float freqParam = params[CUTOFF_PARAM].getValue();
    float freqCvParam = params[CUTOFF_CV_PARAM].getValue();

    int channels = std::max(1, inputs[IN_INPUT].getChannels());


    // Expander handling.  This does not read from analyzer.  Write only.
    bool analyzerPresent = rightExpander.module && rightExpander.module->model == modelPoleDancerWorkbenchForPoleDancerVirtual;
    if (isExpanderClockTick && analyzerPresent) {
      // Write to Analyzer
      PersonalityMessage *toAnalyzer = static_cast<PersonalityMessage*>(rightExpander.module->leftExpander.producerMessage);
      toAnalyzer->leftAuthoritative = true; // always authoritative
      if (inputs[POLE_MIX_INPUT].isConnected()) {
        toAnalyzer->values[0] = poleMix.weight[0];
        toAnalyzer->values[1] = poleMix.weight[1];
        toAnalyzer->values[2] = poleMix.weight[2];
        toAnalyzer->values[3] = poleMix.weight[3];
        toAnalyzer->values[4] = poleMix.weight[4];
      }
      else { // 4-pole lowpass values
        toAnalyzer->values[0] = 0.f;
        toAnalyzer->values[1] = 0.f;
        toAnalyzer->values[2] = 0.f;
        toAnalyzer->values[3] = 0.f;
        toAnalyzer->values[4] = POLEMIX_VOLTAGE_SCALE;
      }

      float rez_ch0 = resParam + inputs[RESONANCE_INPUT].getVoltage(0) / 10.f * resCvParam;
      rez_ch0 = clamp(rez_ch0, 0.f, 1.f);
      toAnalyzer->resonance[3] = rez_ch0;
      toAnalyzer->resonance[2] = 0.f;
      toAnalyzer->resonance[1] = 0.f;
      toAnalyzer->resonance[0] = 0.f;

      rightExpander.module->leftExpander.messageFlipRequested = true;
    }

    if (!outputs[MIX_OUTPUT].isConnected()) {
      return;
    }

    for (int c = 0; c < channels; c += 4) {
      auto& filter = filters[c / 4];

      float_4 input = inputs[IN_INPUT].getVoltageSimd<float_4>(c) / 5.f;

      // Add -120dB noise to bootstrap self-oscillation
      input += 1e-6f * (2.f * random::uniform() - 1.f);

      // Set resonance
      float_4 resonance = resParam + inputs[RESONANCE_INPUT].getPolyVoltageSimd<float_4>(c) / 10.f * resCvParam;
      resonance = clamp(resonance, 0.f, 1.f);
      filter.resonance = simd::pow(resonance, 2) * 10.f;

      // Get pitch
      float_4 pitch = freqParam + inputs[CUTOFF_INPUT].getPolyVoltageSimd<float_4>(c) * freqCvParam;
      // Set cutoff
      float_4 cutoff = dsp::FREQ_C4 * dsp::exp2_taylor5(pitch);
      // Without oversampling, we must limit to 8000 Hz or so @ 44100 Hz
      cutoff = clamp(cutoff, 1.f, args.sampleRate * 0.18f);
      filter.setCutoff(cutoff);

      // Set outputs
      filter.process(input, args.sampleTime);
      if (outputs[MIX_OUTPUT].isConnected()) {
        outputs[MIX_OUTPUT].setVoltageSimd(5.f * filter.poleMix(poleMix), c);
      }
    }

    outputs[MIX_OUTPUT].setChannels(channels);
  }
};


struct PoleDancerWidget : ModuleWidget {
  PoleDancerWidget(PoleDancer* module) {
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/PoleDancer.svg")));

    addChild(createWidget<ScrewSlottedKnurled>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSlottedKnurled>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSlottedKnurled>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSlottedKnurled>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    auto* knob = createParamCentered<TurnsCountingKnob>(
      mm2px(Vec(12.68, 25.48)),
      module,
      PoleDancer::CUTOFF_PARAM);
    knob->setTurns(10);
    knob->turnFromParam = [](float param) -> int {
      // param is in volts, -5V to +5V, octave boundaries at integers
      // floor(param + 5) gives turn 0 at -5V, turn 10 at +5V.
      // Drop by one, so C-1 reads ".1" on the single digit display
      return static_cast<int>(std::floor(param + 5.f) - 1);
    };
    addParam(knob);

    addParam(createParamCentered<ZoxMediumKnob>(mm2px(Vec(33.132, 25.599)), module, PoleDancer::RES_KNOB_PARAM));
    addParam(createParamCentered<ZoxTrimpot>(mm2px(Vec(12.669, 47.72)), module, PoleDancer::CUTOFF_CV_PARAM));
    addParam(createParamCentered<ZoxTrimpot>(mm2px(Vec(33.262, 47.72)), module, PoleDancer::RES_CV_PARAM));

    addInput(createInputCentered<BNCPort>(mm2px(Vec(12.68, 57.45)), module, PoleDancer::CUTOFF_INPUT));
    addInput(createInputCentered<BNCPort>(mm2px(Vec(33.218, 57.45)), module, PoleDancer::RESONANCE_INPUT));
    addInput(createInputCentered<BNCPort>(mm2px(Vec(12.68, 109.193)), module, PoleDancer::IN_INPUT));
    addInput(createInputCentered<BNCPort>(mm2px(Vec(12.68, 91.886)), module, PoleDancer::POLE_MIX_INPUT));

    addOutput(createOutputCentered<BNCPort>(mm2px(Vec(33.218, 109.204)), module, PoleDancer::MIX_OUTPUT));
  }

  void appendContextMenu(Menu *menu) override {

    menu->addChild(new MenuSeparator());

    InstantiateExpanderItem *expanderItem = createMenuItem<InstantiateExpanderItem>("Add Morphscope (right side)", "");
    expanderItem->module = module;
    expanderItem->model = modelPoleDancerWorkbenchForPoleDancerVirtual;
    expanderItem->posit = box.pos;
    expanderItem->posit.x += box.size.x;
    menu->addChild(expanderItem);
  }

};


} // namespace noxious

Model* modelPoleDancerVirtual = createModel<noxious::PoleDancer, noxious::PoleDancerWidget>("PoleDancer");
