/**
 * TCL AC ESPHome component
 * Based on work by Miguel Ángel López, xaxexa, I-am-nightingale
 **/

#ifndef TCL_ESP_TCL_H
#define TCL_ESP_TCL_H

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/climate/climate.h"
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif

#include "tclac_airflow.h"
#include "tclac_protocol.h"

namespace esphome {
namespace tclac {

using climate::ClimateCall;
using climate::ClimateMode;
using climate::ClimatePreset;
using climate::ClimateTraits;
using climate::ClimateFanMode;
using climate::ClimateSwingMode;

class tclacClimate : public climate::Climate, public esphome::uart::UARTDevice, public PollingComponent {

    private:
        uint8_t checksum;
        uint8_t dataTX[38];
        uint8_t dataRX[68];
        uint8_t poll[8] = {0xBB, 0x00, 0x01, 0x04, 0x02, 0x01, 0x00, 0xBD};
        bool beeper_status_{false};
        bool display_status_{true};
        bool allow_take_control{false};
        bool initial_sync_done_{false};

    public:
        tclacClimate() : PollingComponent(5 * 1000) {
            checksum = 0;
            mode = climate::CLIMATE_MODE_OFF;
            swing_mode = climate::CLIMATE_SWING_OFF;
            fan_mode = climate::CLIMATE_FAN_AUTO;
            preset = climate::ClimatePreset::CLIMATE_PRESET_NONE;
            vertical_direction_ = AirflowVerticalDirection::CENTER;
            horizontal_direction_ = AirflowHorizontalDirection::CENTER;
            vertical_swing_direction_ = VerticalSwingDirection::UP_DOWN;
            horizontal_swing_direction_ = HorizontalSwingDirection::LEFT_RIGHT;
        }

        void readData();
        void takeControl();
        void loop() override;
        void update() override;
        void set_beeper_state(bool state);
        void set_display_state(bool disp_state);
        void sendData(uint8_t *message, uint8_t size);
        static uint8_t getChecksum(const uint8_t *message, size_t size);
        void set_vertical_airflow(AirflowVerticalDirection v_airflow);
        void set_horizontal_airflow(AirflowHorizontalDirection h_airflow);
        void set_vertical_swing_direction(VerticalSwingDirection vs_direction);
        void set_horizontal_swing_direction(HorizontalSwingDirection hs_direction);

        AirflowVerticalDirection get_vertical_airflow() const { return vertical_direction_; }
        AirflowHorizontalDirection get_horizontal_airflow() const { return horizontal_direction_; }
        bool get_vertical_swing() const {
            return this->swing_mode == climate::CLIMATE_SWING_VERTICAL || this->swing_mode == climate::CLIMATE_SWING_BOTH;
        }
        bool get_horizontal_swing() const {
            return this->swing_mode == climate::CLIMATE_SWING_HORIZONTAL || this->swing_mode == climate::CLIMATE_SWING_BOTH;
        }
        // Toggle one swing axis without disturbing the other, then resync the AC.
        void set_vertical_swing(bool enabled);
        void set_horizontal_swing(bool enabled);

#ifdef USE_SELECT
        void set_vertical_louver_select(select::Select *s) { vertical_louver_select_ = s; }
        void set_horizontal_louver_select(select::Select *s) { horizontal_louver_select_ = s; }
#endif
        void set_supported_presets(climate::ClimatePresetMask presets);
        void set_supported_modes(climate::ClimateModeMask modes);
        void set_supported_fan_modes(climate::ClimateFanModeMask fan_modes);
        void set_supported_swing_modes(climate::ClimateSwingModeMask swing_modes);

    protected:
        ClimateTraits traits() override;
        climate::ClimateModeMask supported_modes_{};
        AirflowVerticalDirection vertical_direction_;
        climate::ClimatePresetMask supported_presets_{};
        AirflowHorizontalDirection horizontal_direction_;
        VerticalSwingDirection vertical_swing_direction_;
        climate::ClimateFanModeMask supported_fan_modes_{};
        HorizontalSwingDirection horizontal_swing_direction_;
        climate::ClimateSwingModeMask supported_swing_modes_{};
        void control(const climate::ClimateCall &call) override;
        // Push the current swing/louver state to the optional select entities.
        // No-op unless the select platform is compiled in.
        void publish_louver_selects_();
#ifdef USE_SELECT
        select::Select *vertical_louver_select_{nullptr};
        select::Select *horizontal_louver_select_{nullptr};
#endif
};

}  // namespace tclac
}  // namespace esphome

#endif  // TCL_ESP_TCL_H
