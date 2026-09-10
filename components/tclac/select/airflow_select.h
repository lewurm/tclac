#pragma once

#include <string>

#include "esphome/components/select/select.h"
#include "esphome/core/helpers.h"
#include "esphome/components/tclac/tclac.h"

namespace esphome {
namespace tclac {

class VerticalLouverSelect : public select::Select, public Parented<tclacClimate> {
 protected:
  void control(const std::string &value) override;
};

class HorizontalLouverSelect : public select::Select, public Parented<tclacClimate> {
 protected:
  void control(const std::string &value) override;
};

}  // namespace tclac
}  // namespace esphome
