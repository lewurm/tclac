#include "beeper_display_switch.h"

namespace esphome {
namespace tclac {

void BeeperSwitch::write_state(bool state) {
  if (this->parent_->get_beeper_state() != state)
    this->parent_->set_beeper_state(state);
  this->publish_state(state);
}

void DisplaySwitch::write_state(bool state) {
  if (this->parent_->get_display_state() != state)
    this->parent_->set_display_state(state);
  this->publish_state(state);
}

}  // namespace tclac
}  // namespace esphome
