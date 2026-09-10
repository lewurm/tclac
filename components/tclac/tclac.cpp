/**
 * TCL AC ESPHome component — climate entity behaviour.
 * Traits, HA control() translation, runtime setters, louver select mirroring.
 * The UART wire protocol lives in tclac_frame.cpp.
 * Based on work by Miguel Ángel López, xaxexa, I-am-nightingale
 **/
#include "tclac.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace tclac {

static const char *const TAG = "tclac";

ClimateTraits TclacClimate::traits() {
    auto traits = climate::ClimateTraits();
    traits.add_feature_flags(climate::CLIMATE_SUPPORTS_CURRENT_TEMPERATURE);

    if (this->supported_modes_.empty()) {
        traits.add_supported_mode(climate::CLIMATE_MODE_OFF);
        traits.add_supported_mode(climate::CLIMATE_MODE_AUTO);
    } else {
        for (auto mode : this->supported_modes_)
            traits.add_supported_mode(mode);
    }
    if (this->supported_presets_.empty()) {
        traits.add_supported_preset(ClimatePreset::CLIMATE_PRESET_NONE);
    } else {
        for (auto preset : this->supported_presets_)
            traits.add_supported_preset(preset);
    }
    if (this->supported_fan_modes_.empty()) {
        traits.add_supported_fan_mode(climate::CLIMATE_FAN_AUTO);
    } else {
        for (auto fan_mode : this->supported_fan_modes_)
            traits.add_supported_fan_mode(fan_mode);
    }
    if (this->supported_swing_modes_.empty()) {
        traits.add_supported_swing_mode(climate::CLIMATE_SWING_OFF);
    } else {
        for (auto swing_mode : this->supported_swing_modes_)
            traits.add_supported_swing_mode(swing_mode);
    }

    return traits;
}

void TclacClimate::dump_config() {
    LOG_CLIMATE("", "TCLAC Climate", this);
    ESP_LOGCONFIG(TAG, "  Beeper: %s", YESNO(this->beeper_status_));
    ESP_LOGCONFIG(TAG, "  Display: %s", YESNO(this->display_status_));
    this->check_uart_settings(9600, 1, uart::UART_CONFIG_PARITY_EVEN, 8);
    LOG_UPDATE_INTERVAL(this);
}

void TclacClimate::control(const climate::ClimateCall &call) {
    if (call.get_mode().has_value())               this->mode               = *call.get_mode();
    if (call.get_target_temperature().has_value()) this->target_temperature = *call.get_target_temperature();
    if (call.get_fan_mode().has_value())           this->fan_mode           = *call.get_fan_mode();
    if (call.get_swing_mode().has_value())         this->swing_mode         = *call.get_swing_mode();
    if (call.get_preset().has_value())             this->preset             = *call.get_preset();

    this->publish_state();
    this->publish_louver_selects_();
    this->takeControl();
    this->allow_take_control = true;
}

void TclacClimate::set_beeper_state(bool state) {
    beeper_status_ = state;
    this->publish_switch_states_();
    if (allow_take_control) takeControl();
}

void TclacClimate::set_display_state(bool disp_state) {
    display_status_ = disp_state;
    this->publish_switch_states_();
    if (allow_take_control) takeControl();
}

void TclacClimate::set_vertical_airflow(AirflowVerticalDirection v_airflow) {
    vertical_direction_ = v_airflow;
    this->publish_louver_selects_();
    if (allow_take_control) takeControl();
}

void TclacClimate::set_horizontal_airflow(AirflowHorizontalDirection h_airflow) {
    horizontal_direction_ = h_airflow;
    this->publish_louver_selects_();
    if (allow_take_control) takeControl();
}

void TclacClimate::set_vertical_swing_direction(VerticalSwingDirection vs_direction) {
    vertical_swing_direction_ = vs_direction;
    if (allow_take_control) takeControl();
}

void TclacClimate::set_horizontal_swing_direction(HorizontalSwingDirection hs_direction) {
    horizontal_swing_direction_ = hs_direction;
    if (allow_take_control) takeControl();
}

void TclacClimate::set_vertical_swing(bool enabled) {
    bool h = this->get_horizontal_swing();
    this->swing_mode = enabled ? (h ? climate::CLIMATE_SWING_BOTH : climate::CLIMATE_SWING_VERTICAL)
                               : (h ? climate::CLIMATE_SWING_HORIZONTAL : climate::CLIMATE_SWING_OFF);
    this->publish_state();
    this->publish_louver_selects_();
    if (allow_take_control) takeControl();
}

void TclacClimate::set_horizontal_swing(bool enabled) {
    bool v = this->get_vertical_swing();
    this->swing_mode = enabled ? (v ? climate::CLIMATE_SWING_BOTH : climate::CLIMATE_SWING_HORIZONTAL)
                               : (v ? climate::CLIMATE_SWING_VERTICAL : climate::CLIMATE_SWING_OFF);
    this->publish_state();
    this->publish_louver_selects_();
    if (allow_take_control) takeControl();
}

#ifdef USE_SELECT
static const char *vertical_airflow_option(AirflowVerticalDirection dir) {
    switch (dir) {
        case AirflowVerticalDirection::MAX_UP:   return "Max up";
        case AirflowVerticalDirection::UP:       return "Up";
        case AirflowVerticalDirection::DOWN:     return "Down";
        case AirflowVerticalDirection::MAX_DOWN: return "Max down";
        default:                                 return "Center";
    }
}

static const char *horizontal_airflow_option(AirflowHorizontalDirection dir) {
    switch (dir) {
        case AirflowHorizontalDirection::MAX_LEFT:  return "Max left";
        case AirflowHorizontalDirection::LEFT:      return "Left";
        case AirflowHorizontalDirection::RIGHT:     return "Right";
        case AirflowHorizontalDirection::MAX_RIGHT: return "Max right";
        default:                                    return "Center";
    }
}
#endif

void TclacClimate::publish_louver_selects_() {
#ifdef USE_SELECT
    if (this->vertical_louver_select_ != nullptr) {
        this->vertical_louver_select_->publish_state(
            this->get_vertical_swing() ? "Swing" : vertical_airflow_option(this->vertical_direction_));
    }
    if (this->horizontal_louver_select_ != nullptr) {
        this->horizontal_louver_select_->publish_state(
            this->get_horizontal_swing() ? "Swing" : horizontal_airflow_option(this->horizontal_direction_));
    }
#endif
}

void TclacClimate::publish_switch_states_() {
#ifdef USE_SWITCH
    if (this->beeper_switch_ != nullptr)
        this->beeper_switch_->publish_state(this->beeper_status_);
    if (this->display_switch_ != nullptr)
        this->display_switch_->publish_state(this->display_status_);
#endif
}

void TclacClimate::set_supported_modes(climate::ClimateModeMask modes) {
    supported_modes_ = modes;
}

void TclacClimate::set_supported_fan_modes(climate::ClimateFanModeMask fan_modes) {
    supported_fan_modes_ = fan_modes;
}

void TclacClimate::set_supported_swing_modes(climate::ClimateSwingModeMask swing_modes) {
    supported_swing_modes_ = swing_modes;
}

void TclacClimate::set_supported_presets(climate::ClimatePresetMask presets) {
    supported_presets_ = presets;
}

}  // namespace tclac
}  // namespace esphome
