#pragma once

// Byte offsets and bit masks of the TCL UART protocol, shared by the frame
// decoder (readData) and encoder (takeControl) in tclac_frame.cpp.

#define SET_TEMP_MASK   0b00001111

#define MODE_POS        7
// Bit 0b00100000 in the mode byte is the AC's DISPLAY state, not part of the
// mode — it must not be included in MODE_MASK, otherwise turning the display
// off makes every mode read as unrecognized and fall back to default (AUTO).
#define DISPLAY_BIT     0b00100000
#define MODE_MASK       0b00001111

#define MODE_AUTO       0b00000101
#define MODE_COOL       0b00000001
#define MODE_DRY        0b00000011
#define MODE_FAN_ONLY   0b00000010
#define MODE_HEAT       0b00000100

#define FAN_SPEED_POS   8
#define FAN_QUIET_POS   33

#define FAN_AUTO        0b10000000
#define FAN_QUIET       0x80
#define FAN_LOW         0b10010000
#define FAN_MIDDLE      0b11000000
#define FAN_MEDIUM      0b10100000
#define FAN_HIGH        0b11010000
#define FAN_FOCUS       0b10110000
#define FAN_DIFFUSE     0b10000000
#define FAN_SPEED_MASK  0b11110000

#define SWING_POS           10
#define SWING_OFF           0b00000000
#define SWING_HORIZONTAL    0b00100000
#define SWING_VERTICAL      0b01000000
#define SWING_BOTH          0b01100000
#define SWING_MODE_MASK     0b01100000

// Experimental diagnostic fields. These offsets are present in every accepted
// response frame (the shortest valid frame is 61 bytes), but their semantics
// have only been verified on a TAC-12CHSD/HCI.
#define DIAGNOSTIC_START_POS        34
#define COMPRESSOR_FREQUENCY_POS    39
#define COMPRESSOR_RUNNING_POS      46
#define DIAGNOSTIC_END_POS          59
