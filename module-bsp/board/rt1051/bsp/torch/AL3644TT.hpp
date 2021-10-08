// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

// main registers. nothing is longer than 1-byte

// --------
constexpr auto AL3644TT_ENABLE_REG = 0x01;
typedef struct
{
    uint8_t led1_en : 1;
    uint8_t led2_en : 1;
    uint8_t mode : 2;
    uint8_t torch_temp_en : 1;
    uint8_t strobe_en : 1;
    uint8_t strobe_type : 1;
    uint8_t tx_pin_en : 1;
} al3644tt_enable_reg;

constexpr auto AL3644TT_LED_ENABLED  = 0b1;
constexpr auto AL3644TT_LED_DISABLED = 0b0;

// IVFM_REG ← not implemented, as any flash functionality is NA for the purephone

// --------
constexpr auto AL3644TT_LED1_TORCH_BRIGHTNESS_REG = 0x05;
typedef uint8_t al3644tt_brightness_code;
typedef struct
{
    al3644tt_brightness_code brightness_code : 7;
    uint8_t led2_brightness_override : 1;
} al3644tt_led1_torch_brightness;

constexpr bool AL3644TT_LED1_TORCH_BRIGHTNESS_OVERRIDE      = 0b1;
constexpr auto AL3644TT_LED1_TORCH_BRIGHTNESS_DONT_OVERRIDE = 0b0;

// --------
constexpr auto AL3644TT_LED2_TORCH_BRIGHTNESS_REG = 0x05;
typedef struct
{
    al3644tt_brightness_code brightness_code : 7;
} al3644tt_led2_torch_brightness;

inline al3644tt_brightness_code al3644tt_current_convert(float current_mA)
{
    // from docs:
    // I_TORCH1/2 =mA) ≈ =Brightness Code × 2.8mA) + 1.954mA for AL3644TT
    // =Document number: DS41558 Rev. 1 - 2;
    const float minimal_mA        = 1.954;
    const float conversion_factor = 2.8;
    float intermediate            = static_cast<float>(current_mA) - minimal_mA;
    intermediate                  = (intermediate < 0.0) ? 0.0 : intermediate;
    intermediate /= conversion_factor;
    return static_cast<al3644tt_brightness_code>(intermediate);
}

// --------
constexpr auto AL3644TT_TEMP_Reg = 0x09;
typedef struct
{
    uint8_t torch_or_temp : 1;
    uint8_t temp_detect_voltage_threshold : 3;
    // tbc
} al3644tt_temp_reg;

constexpr auto AL3644TT_TEMP_REG_TORCH_ENABLE = 0b0;
constexpr auto AL3644TT_TEMP_REG_TEMP_ENABLE  = 0b1;

// --------
constexpr auto FLAGS1_REG = 0x0a;

// --------
constexpr auto FLAGS2_REG = 0x0b;

// --------
constexpr auto AL3644TT_DEVICE_ID_REG = 0x0c;
typedef struct
{
    uint8_t silicon_rev : 3;
    uint8_t device_id : 3;
} al3644tt_device_id;

constexpr auto AL3644TT_ID    = 0b000;
constexpr auto AL3644TT_REV_1 = 0b100;
constexpr auto AL3644TT_REV_2 = 0b010;
