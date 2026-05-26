#pragma once
#include "componentlibrary.hpp"

namespace noxious {
  
// plain simple UI skinning for ports and screws

struct BNCPort : app::SvgPort {
  BNCPort() {
    setSvg(Svg::load(asset::plugin(pluginInstance, "res/BNCFemale.svg")));
  }
};


struct ScrewSlottedKnurled : app::SvgScrew {
  ScrewSlottedKnurled() {
    setSvg(Svg::load(asset::plugin(pluginInstance, "res/ScrewSlottedKnurled.svg")));
  }
};


// colors for lights within the style

struct ZoxAmberLight : GrayModuleLightWidget {
  ZoxAmberLight() {
    addBaseColor(nvgRGB(0xff, 0x9a, 0x35));
  }
};



//
// ZPushButtons -- Medium and Small
//

struct ZPushButtonSmallSvg {
  static constexpr const char* unlatched = "res/ZPushButtonSmall_unlatched.svg";
  static constexpr const char* latched   = "res/ZPushButtonSmall_latched.svg";
};

struct ZPushButtonSmallLeftSvg {
  static constexpr const char* unlatched = "res/ZPushButtonSmall_left_unlatched.svg";
  static constexpr const char* latched   = "res/ZPushButtonSmall_latched.svg";
};

struct ZPushButtonSmallRightSvg {
  static constexpr const char* unlatched = "res/ZPushButtonSmall_right_unlatched.svg";
  static constexpr const char* latched   = "res/ZPushButtonSmall_latched.svg";
};

struct ZPushButtonSmallUpSvg {
  static constexpr const char* unlatched = "res/ZPushButtonSmall_up_unlatched.svg";
  static constexpr const char* latched   = "res/ZPushButtonSmall_latched.svg";
};

struct ZPushButtonSmallDownSvg {
  static constexpr const char* unlatched = "res/ZPushButtonSmall_down_unlatched.svg";
  static constexpr const char* latched   = "res/ZPushButtonSmall_latched.svg";
};


struct ZPushButtonMediumSvg {
  static constexpr const char* unlatched = "res/ZPushButtonMedium_unlatched.svg";
  static constexpr const char* latched   = "res/ZPushButtonMedium_latched.svg";
};

struct ZPushButtonMediumLeftSvg {
  static constexpr const char* unlatched = "res/ZPushButtonMedium_left_unlatched.svg";
  static constexpr const char* latched   = "res/ZPushButtonMedium_latched.svg";
};

struct ZPushButtonMediumRightSvg {
  static constexpr const char* unlatched = "res/ZPushButtonMedium_right_unlatched.svg";
  static constexpr const char* latched   = "res/ZPushButtonMedium_latched.svg";
};

struct ZPushButtonMediumUpSvg {
  static constexpr const char* unlatched = "res/ZPushButtonMedium_up_unlatched.svg";
  static constexpr const char* latched   = "res/ZPushButtonMedium_latched.svg";
};

struct ZPushButtonMediumDownSvg {
  static constexpr const char* unlatched = "res/ZPushButtonMedium_down_unlatched.svg";
  static constexpr const char* latched   = "res/ZPushButtonMedium_latched.svg";
};




template <typename TSvg>
struct ZPushButton : app::SvgSwitch {
  ZPushButton() {
    momentary = true;
    addFrame(Svg::load(asset::plugin(pluginInstance, TSvg::unlatched)));
    addFrame(Svg::load(asset::plugin(pluginInstance, TSvg::latched)));
  }
};


// Stateful latch, no light:
template <typename TSvg>
struct ZPushButtonStatefulLatch : app::SvgSwitch {
  std::shared_ptr<Svg> offSvg;
  std::shared_ptr<Svg> onSvg;

  ZPushButtonStatefulLatch() {
    momentary = false;
    latch = true;

    offSvg = Svg::load(asset::plugin(pluginInstance, TSvg::unlatched));
    onSvg  = Svg::load(asset::plugin(pluginInstance, TSvg::latched));

    addFrame(offSvg);
    addFrame(onSvg);
  }

  void step() override {
    app::SvgSwitch::step();

    engine::ParamQuantity* pq = this->getParamQuantity();
    if (pq) {
      bool on = pq->getValue() > 0.5f;
        if (on) {
          this->sw->setSvg(onSvg);
          this->shadow->opacity = 0.f;
        }
        else {
          this->sw->setSvg(offSvg);
          this->shadow->opacity = 0.15f;
        }
    }
  }
};



// stateful latch with light:
template <typename TSvg, typename TLight = MediumSimpleLight<WhiteLight>>
  struct ZPushButtonStatefulLightLatch : app::SvgSwitch {
    app::ModuleLightWidget* light;

    std::shared_ptr<Svg> offSvg;
    std::shared_ptr<Svg> onSvg;

    ZPushButtonStatefulLightLatch() {
      momentary = false;
      latch = true;

      offSvg = Svg::load(asset::plugin(pluginInstance, TSvg::unlatched));
      onSvg  = Svg::load(asset::plugin(pluginInstance, TSvg::latched));

      addFrame(offSvg);
      addFrame(onSvg);

      light = new TLight;
      light->box.pos = box.size.div(2).minus(light->box.size.div(2));
      addChild(light);
    }

    void step() override {
      app::SvgSwitch::step();

      engine::ParamQuantity* pq = this->getParamQuantity();
      if (pq) {
        bool on = pq->getValue() > 0.5f;
        if (on) {
          this->sw->setSvg(onSvg);
          this->shadow->opacity = 0.f;
        }
        else {
          this->sw->setSvg(offSvg);
          this->shadow->opacity = 0.15f;
        }
      }
    }

    app::ModuleLightWidget* getLight() {
      return light;
    }
};


// names to reference these by for use:
using ZPushButtonSmall = ZPushButton<ZPushButtonSmallSvg>;
using ZPushButtonMedium = ZPushButton<ZPushButtonMediumSvg>;

template <typename TLight = WhiteLight>
using ZLightPushButtonSmall = LightButton<ZPushButtonSmall, TLight>;

template <typename TLight = WhiteLight>
using ZLightPushButtonMedium = LightButton<ZPushButtonMedium, TLight>;

template <typename TLight>
using ZPushButtonSmallStatefulLightLatch =
	ZPushButtonStatefulLightLatch<ZPushButtonSmallSvg, TLight>;

template <typename TLight>
using ZPushButtonMediumStatefulLightLatch =
	ZPushButtonStatefulLightLatch<ZPushButtonMediumSvg, TLight>;

using ZPushButtonSmallStatefulLatch =
	ZPushButtonStatefulLatch<ZPushButtonSmallSvg>;

using ZPushButtonMediumStatefulLatch =
	ZPushButtonStatefulLatch<ZPushButtonMediumSvg>;

using ZPushButtonSmallLeft = ZPushButton<ZPushButtonSmallLeftSvg>;
using ZPushButtonSmallRight = ZPushButton<ZPushButtonSmallRightSvg>;
using ZPushButtonSmallUp = ZPushButton<ZPushButtonSmallUpSvg>;
using ZPushButtonSmallDown = ZPushButton<ZPushButtonSmallDownSvg>;
using ZPushButtonMediumLeft = ZPushButton<ZPushButtonMediumLeftSvg>;
using ZPushButtonMediumRight = ZPushButton<ZPushButtonMediumRightSvg>;
using ZPushButtonMediumUp = ZPushButton<ZPushButtonMediumUpSvg>;
using ZPushButtonMediumDown = ZPushButton<ZPushButtonMediumDownSvg>;




static constexpr float backgroundWidth = 2.5f;
static constexpr float x = backgroundWidth / 2.f;
static constexpr float grooveBottom = 62.f;
static constexpr float grooveTop = -5.f;
static constexpr float handleHeight = 0.f;
static constexpr float yMin = grooveBottom - handleHeight / 2.f;
static constexpr float yMax = grooveTop    + handleHeight / 2.f;

// slider model
struct ZoxSlider : app::SvgSlider {
  ZoxSlider() {
    setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/ZoxSlider.svg")));
    setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/ZoxSliderHandle.svg")));
    setHandlePos(math::Vec(x, yMin), math::Vec(x, yMax));
  }
};


} // namespace noxious
