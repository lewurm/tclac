#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/helpers.h"
#include "esphome/components/tclac/tclac.h"

namespace esphome {
namespace tclac {

class BeeperSwitch : public switch_::Switch, public Parented<TclacClimate> {
 protected:
  void write_state(bool state) override;
};

class DisplaySwitch : public switch_::Switch, public Parented<TclacClimate> {
 protected:
  void write_state(bool state) override;
};

}  // namespace tclac
}  // namespace esphome
