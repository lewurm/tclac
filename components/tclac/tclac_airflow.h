#pragma once

#include <cstdint>

namespace esphome {
namespace tclac {

// Fixed louver / airflow positions and swing ranges. Kept separate from tclac.h
// so the select platform (select/airflow_select.*) can pull just these without
// the whole climate class.

enum class VerticalSwingDirection : uint8_t {
    UP_DOWN = 0,
    UPSIDE = 1,
    DOWNSIDE = 2,
};
enum class HorizontalSwingDirection : uint8_t {
    LEFT_RIGHT = 0,
    LEFTSIDE = 1,
    CENTER = 2,
    RIGHTSIDE = 3,
};
enum class AirflowVerticalDirection : uint8_t {
    LAST = 0,
    MAX_UP = 1,
    UP = 2,
    CENTER = 3,
    DOWN = 4,
    MAX_DOWN = 5,
};
enum class AirflowHorizontalDirection : uint8_t {
    LAST = 0,
    MAX_LEFT = 1,
    LEFT = 2,
    CENTER = 3,
    RIGHT = 4,
    MAX_RIGHT = 5,
};

}  // namespace tclac
}  // namespace esphome
