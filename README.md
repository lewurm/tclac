# tclac — ESPHome External Component for TCL AC Units

ESPHome external component for controlling TCL air conditioners and compatible units via UART.

This is a fork of [I-am-nightingale/tclac](https://github.com/I-am-nightingale/tclac), rewritten to follow native ESPHome component conventions.

## Changes from upstream

- Removed packages-based configuration in favor of `external_components:` with a self-contained device YAML
- Reworked the runtime helper entities: dropped the upstream Force-config switch and swing selects; the beeper / display switches and the louver selects are back as optional `switch:` / `select:` platforms (see below), everything else is compile-time
- Removed LED wiring and screen management logic unrelated to the AC protocol
- Removed `force_mode` and `module_display` options
- Fixed temperature precision (integer division bug in sensor formula)
- Fixed uninitialized enum members in constructor (undefined behavior)
- Added `initial_sync_done_` pattern: pushes compile-time `beeper` and `show_display` settings to the AC on the first UART response
- Converted `climate.py` to async `to_code()`, applied all airflow/swing compile-time options
- Fixed `VerticalSwingDirection` enum name mismatch between Python and C++ (`UPDOWN` -> `UP_DOWN`)
- Made the component framework-neutral: builds warning-free under both the Arduino and ESP-IDF frameworks; added a GitHub Actions matrix (`tests/test.esp32-*.yaml`) that compiles it on every push
- Added an optional `select:` platform exposing runtime **vertical/horizontal louver** entities; per axis the AC either sweeps (swing) or holds a fixed vane position — never both — and `takeControl()` now sends only the field that matches the current mode. The vane positions and swing ranges are persisted to flash (`setup()`), since the AC never reports them back
- Added an optional `switch:` platform with **Beeper** and **Display** toggles; `show_display` is also synced back from the AC when the physical remote changes it
- Fixed the `climate.tclac.set_{vertical,horizontal}_swing_direction` actions (Python called `set_swing_direction()`, the C++ `TEMPLATABLE_VALUE` macro exposes `set_direction()`)
- Split the C++ by concern: `tclac_protocol.h` (byte offsets / bit masks), `tclac_airflow.h` (louver enums), `tclac_frame.cpp` (UART read/build), `tclac.cpp` (entity behaviour)

## Hardware

Tested on **M5Stack ATOM S3 Lite** (ESP32-S3).

UART wiring:
- RX: GPIO7
- TX: GPIO8

The AC unit must expose a UART interface. Depending on the model, this may require soldering or using the native WiFi module connector. Even units with the same model number may differ in PCB revision and connector availability.

## Protocol

- Baud rate: 9600
- Data bits: 8
- Parity: EVEN
- Stop bits: 1
- RX packet: 68 bytes, XOR checksum over bytes [0..66] = byte[67]
- Poll packet sent every 5 seconds: `BB 00 01 04 02 01 00 BD`
- TX command packet: 38 bytes, same XOR checksum scheme

## Supported hardware

Verified on this fork:

- TCL TAC-12CHSD/UG11V3AH

Verified compatible units (from upstream):

- Axioma ASX09H1 / ASB09H1
- Ballu Discovery DC BSVI-07HN8, BSVI-09HN8, BSVI-12HN8
- Daichi AIR20AVQ1 / AIR20FV1
- Daichi AIR25AVQS1R-1 / AIR25FVS1R-1
- Daichi AIR35AVQS1R-1 / AIR35FVS1R-1
- Daichi DA35EVQ1-1 / DF35EV1-1
- Dantex RK-12SATI / RK-12SATIE
- Ecostar Radium KVS-RAD09CH
- Royal Clima Gloria Inverter
- TCL ELI ONF 12
- TCL Liferise ONF 09
- TCL TAC-CT09INV/R
- TCL One Inverter TACM-09HRID/E1
- TCL TAC-07CHSA/TPG-W
- TCL TAC-09CHSA/TPG
- TCL TAC-09CHSA/DSEI-W
- TCL TAC-09HRID/E1
- TCL TAC-12CHSA/TPG
- TCL TAC-12CHSA/TPGI
- TCL TAC-XAL24I
- TCL TPG31IHB

## Configuration

```yaml
esphome:
  name: tcl-ac
  friendly_name: TCL AC
  min_version: 2026.4.0

esp32:
  board: m5stack-atoms3
  variant: esp32s3
  framework:
    type: arduino

logger:
  level: DEBUG
  baud_rate: 0

api:
  encryption:
    key: !secret api_key

ota:
  - platform: esphome

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

external_components:
  - source:
      url: https://github.com/rozenofu/tclac.git
      type: git
      ref: master
    components: [ tclac ]
    refresh: 30s

uart:
  id: ac_uart
  rx_pin: GPIO7
  tx_pin: GPIO8
  baud_rate: 9600
  data_bits: 8
  parity: EVEN
  stop_bits: 1

climate:
  - platform: tclac
    name: "TCL AC"
    uart_id: ac_uart
    beeper: false
    show_display: true
    supported_modes:
      - "OFF"
      - AUTO
      - COOL
      - HEAT
      - DRY
      - FAN_ONLY
    supported_fan_modes:
      - AUTO
      - QUIET
      - LOW
      - MIDDLE
      - MEDIUM
      - HIGH
      - FOCUS
      - DIFFUSE
    supported_swing_modes:
      - "OFF"
      - VERTICAL
      - HORIZONTAL
      - BOTH
```

### Climate options

| Option | Type | Default | Description |
|---|---|---|---|
| `beeper` | bool | `false` | Enable AC beeper on command |
| `show_display` | bool | `true` | Enable AC display panel |
| `supported_modes` | list | all | Modes exposed in HA |
| `supported_fan_modes` | list | all | Fan speeds exposed in HA |
| `supported_swing_modes` | list | all | Swing modes exposed in HA |
| `supported_presets` | list | all | Presets exposed in HA (NONE, ECO, SLEEP, COMFORT) |
| `vertical_airflow` | enum | `CENTER` | Fixed vertical louver position (LAST, MAX_UP, UP, CENTER, DOWN, MAX_DOWN) |
| `horizontal_airflow` | enum | `CENTER` | Fixed horizontal louver position (LAST, MAX_LEFT, LEFT, CENTER, RIGHT, MAX_RIGHT) |
| `vertical_swing_mode` | enum | `UP_DOWN` | Vertical swing range (UP_DOWN, UPSIDE, DOWNSIDE) |
| `horizontal_swing_mode` | enum | `LEFT_RIGHT` | Horizontal swing range (LEFT_RIGHT, LEFTSIDE, CENTER, RIGHTSIDE) |

Note: `"OFF"` in `supported_modes` and `supported_swing_modes` must be quoted. YAML parses bare `OFF` as boolean false.

### Louver select entities

The optional `select:` platform adds one dropdown entity per axis. Each has a
`Swing` option plus the fixed vane positions. Picking `Swing` re-enables that
axis on the climate entity's `swing_mode`; picking a fixed position clears it.
Changing `swing_mode` on the climate entity keeps the selects in sync.

```yaml
select:
  - platform: tclac
    tclac_id: ac            # id: of the climate entity
    vertical_louver:
      name: "TCL AC Vertical Louver"
    horizontal_louver:
      name: "TCL AC Horizontal Louver"
```

| Key | Options |
| --- | --- |
| `vertical_louver` | `Swing`, `Max up`, `Up`, `Center`, `Down`, `Max down` |
| `horizontal_louver` | `Swing`, `Max left`, `Left`, `Center`, `Right`, `Max right` |

State is optimistic: the AC status frame does not report vane position, so the
selects reflect the last commanded value, not a read-back. That value (and the
two swing-range settings) is persisted to flash and re-applied on the next
boot — otherwise an ESP reboot would silently reset the vanes to the YAML
defaults.

### Beeper / display switches

The optional `switch:` platform exposes the AC beeper and front-panel display as
runtime toggles. The climate `beeper` / `show_display` options stay as the boot
defaults; the switches let you change them from HA afterwards.

```yaml
switch:
  - platform: tclac
    tclac_id: ac            # id: of the climate entity
    beeper:
      name: "TCL AC Beeper"
    display:
      name: "TCL AC Display"
```

`beeper` is optimistic (the AC never reports it back). `display` is synced from
the status frame, so it follows changes made with the physical remote.

### Automation actions

```yaml
# Beeper
- climate.tclac.beeper_on:
- climate.tclac.beeper_off:

# Display
- climate.tclac.display_on:
- climate.tclac.display_off:

# Louver position at runtime
- climate.tclac.set_vertical_airflow:
    vertical_airflow: DOWN
- climate.tclac.set_horizontal_airflow:
    horizontal_airflow: CENTER
- climate.tclac.set_vertical_swing_direction:
    vertical_swing_mode: UP_DOWN
- climate.tclac.set_horizontal_swing_direction:
    horizontal_swing_mode: LEFT_RIGHT
```

---

## Credits

Protocol reverse engineering and original implementation: [@I-am-nightingale](https://github.com/I-am-nightingale/tclac), [@xaxexa](https://github.com/xaxexa), [@junkfix](https://github.com/junkfix).

The protocol analysis, byte-level decoding of AC state packets, and command frame structure are based entirely on the upstream work.
