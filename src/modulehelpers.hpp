#pragma once
#include <string>
#include "plugin.hpp"

namespace noxious {

static constexpr float POLEMIX_VOLTAGE_SCALE = 10.0f / 32.f;
static constexpr int EXPANDER_CLOCK_DIV = 256;

// filter vector:
// struct that gets passed between modules via expander interface
struct PersonalityMessage {
  float values[5] = {};
  float resonance[4] = {};
  bool leftAuthoritative = false;
};


// for adding a module via right-click menu
struct InstantiateExpanderItem : MenuItem {
  Module* module;
  Model* model;
  Vec posit;
  void onAction(const event::Action &e) override;
};

} // namespace noxious
