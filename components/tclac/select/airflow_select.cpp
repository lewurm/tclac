#include "airflow_select.h"

namespace esphome {
namespace tclac {

void VerticalLouverSelect::control(const std::string &value) {
  if (value == "Swing") {
    this->parent_->set_vertical_swing(true);
  } else {
    AirflowVerticalDirection dir = AirflowVerticalDirection::CENTER;
    if (value == "Max up") {
      dir = AirflowVerticalDirection::MAX_UP;
    } else if (value == "Up") {
      dir = AirflowVerticalDirection::UP;
    } else if (value == "Down") {
      dir = AirflowVerticalDirection::DOWN;
    } else if (value == "Max down") {
      dir = AirflowVerticalDirection::MAX_DOWN;
    }
    this->parent_->set_vertical_airflow(dir);
    this->parent_->set_vertical_swing(false);
  }
  this->publish_state(value);
}

void HorizontalLouverSelect::control(const std::string &value) {
  if (value == "Swing") {
    this->parent_->set_horizontal_swing(true);
  } else {
    AirflowHorizontalDirection dir = AirflowHorizontalDirection::CENTER;
    if (value == "Max left") {
      dir = AirflowHorizontalDirection::MAX_LEFT;
    } else if (value == "Left") {
      dir = AirflowHorizontalDirection::LEFT;
    } else if (value == "Right") {
      dir = AirflowHorizontalDirection::RIGHT;
    } else if (value == "Max right") {
      dir = AirflowHorizontalDirection::MAX_RIGHT;
    }
    this->parent_->set_horizontal_airflow(dir);
    this->parent_->set_horizontal_swing(false);
  }
  this->publish_state(value);
}

}  // namespace tclac
}  // namespace esphome
