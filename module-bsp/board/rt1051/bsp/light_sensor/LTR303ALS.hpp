// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace bsp::light_sensor
{
    constexpr inline auto LTR303ALS_DEVICE_ADDR = 0x29;

    enum class LTR303ALS_Registers
    {
        ALS_CONTR         = 0x80,
        ALS_MEAS_RATE     = 0x85,
        PART_ID           = 0x86,
        MANUFAC_ID        = 0x87,
        ALS_DATA_CH1_0    = 0x88,
        ALS_DATA_CH1_1    = 0x89,
        ALS_DATA_CH0_0    = 0x8A,
        ALS_DATA_CH0_1    = 0x8B,
        ALS_STATUS        = 0x8C,
        INTERRUPT         = 0x8F,
        ALS_THRES_UP_0    = 0x97,
        ALS_THRES_UP_1    = 0x98,
        ALS_THRES_LOW_0   = 0x99,
        ALS_THRES_LOW_1   = 0x9A,
        INTERRUPT_PERSIST = 0x9E
    };

    constexpr inline auto ACTIVE_MODE = 0b00000001;
    constexpr inline auto SW_RESET    = 0b00000010;

    constexpr inline auto INTEGRATION_TIME_200MS = 0b00010000;
    constexpr inline auto ALS_INT_200MS          = 2;
    constexpr inline auto INTEGRATION_TIME_400MS = 0b00011000;
    constexpr inline auto ALS_INT_400MS          = 4;

    constexpr inline auto ALS_GAIN = 1.0f;
    // Factor of light resistance of window above the sensor - roughly estimated value
    constexpr inline auto PFACTOR           = 0.8f;
    constexpr inline auto MEASUREMENT_COEFF = (ALS_INT_200MS * ALS_GAIN) / PFACTOR;

    constexpr inline auto MEASUREMENT_RATE_500MS  = 0b00000011;
    constexpr inline auto MEASUREMENT_RATE_2000MS = 0b00000111;

    constexpr inline auto ENABLE_IRQ = 0x0A;

    constexpr inline auto MANUFACTURER_ID = 0x05;
    constexpr inline auto LUX_RANGE       = 64000.f;

    constexpr inline float decodeLightMeasurement(std::uint8_t *data)
    {
        std::uint16_t encodedCh1 = (static_cast<std::uint16_t>(data[1]) << 8) | data[0];
        std::uint16_t encodedCh0 = (static_cast<std::uint16_t>(data[3]) << 8) | data[2];

        if (encodedCh0 == 0 || encodedCh1 == 0) {
            return 0.0f;
        }
        float ratio = encodedCh1 / (encodedCh1 + encodedCh0);

        if (ratio < 0.45f) {
            return (1.7743 * encodedCh0 + 1.1059 * encodedCh1) / MEASUREMENT_COEFF;
        }
        else if (ratio < 0.64 && ratio >= 0.45) {
            return (4.2785 * encodedCh0 - 1.9548 * encodedCh1) / MEASUREMENT_COEFF;
        }
        else if (ratio < 0.85 && ratio >= 0.64) {
            return (0.5926 * encodedCh0 + 0.1185 * encodedCh1) / MEASUREMENT_COEFF;
        }
        else {
            return 0.0f;
        }
    }

} // namespace bsp::light_sensor
