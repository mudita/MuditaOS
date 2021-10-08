// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

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

    constexpr inline auto MEASUREMENT_GAIN_8 = 0b00001100;
    constexpr inline auto ACTIVE_MODE        = 0b00000001 | MEASUREMENT_GAIN_8;
    constexpr inline auto SW_RESET    = 0b00000010;

    constexpr inline auto INTEGRATION_TIME_50MS  = 0b00001000;
    constexpr inline auto ALS_INT_50MS           = 0.5f;
    constexpr inline auto INTEGRATION_TIME_100MS = 0b00000000;
    constexpr inline auto ALS_INT_100MS          = 1;
    constexpr inline auto INTEGRATION_TIME_200MS = 0b00010000;
    constexpr inline auto ALS_INT_200MS          = 2;
    constexpr inline auto INTEGRATION_TIME_400MS = 0b00011000;
    constexpr inline auto ALS_INT_400MS          = 4;

    constexpr inline auto MEASUREMENT_RATE_50MS   = 0b00000000;
    constexpr inline auto MEASUREMENT_RATE_100MS  = 0b00000001;
    constexpr inline auto MEASUREMENT_RATE_500MS  = 0b00000011;
    constexpr inline auto MEASUREMENT_RATE_1000MS = 0b00000100;
    constexpr inline auto MEASUREMENT_RATE_2000MS = 0b00000111;

    constexpr inline auto MEAS_SETUP = INTEGRATION_TIME_400MS | MEASUREMENT_RATE_500MS;

    constexpr inline auto ALS_GAIN = 8.0f;
    // Factor of light resistance of window above the sensor - roughly estimated value
    constexpr inline auto PFACTOR           = 0.8f;
    constexpr inline auto MEASUREMENT_COEFF = (ALS_INT_400MS * ALS_GAIN) / PFACTOR;

    constexpr inline auto ENABLE_IRQ = 0x0A;

    constexpr inline auto MANUFACTURER_ID = 0x05;
    constexpr inline auto LUX_RANGE       = 64000.f;

    constexpr float decodeLightMeasurement(const std::uint8_t *data)
    {
        const auto ch1 = static_cast<float>((static_cast<std::uint16_t>(data[1]) << 8) | data[0]);
        const auto ch0 = static_cast<float>((static_cast<std::uint16_t>(data[3]) << 8) | data[2]);

        constexpr auto ratio1Threshold = 0.45f;
        constexpr auto ratio2Threshold = 0.64f;
        constexpr auto ratio3Threshold = 0.85f;
        constexpr auto ch0CoeffRatio1  = 1.7743f;
        constexpr auto ch1CoeffRatio1  = 1.1059f;
        constexpr auto ch0CoeffRatio2  = 4.2785f;
        constexpr auto ch1CoeffRatio2  = 1.9548f;
        constexpr auto ch0CoeffRatio3  = 0.5926f;
        constexpr auto ch1CoeffRatio3  = 0.1185f;

        if (ch0 == 0.0f || ch1 == 0.0f) {
            return 0.0f;
        }
        float ratio = ch1 / (ch1 + ch0);

        if (ratio < ratio1Threshold) {
            return (ch0CoeffRatio1 * ch0 + ch1CoeffRatio1 * ch1) / MEASUREMENT_COEFF;
        }
        else if (ratio < ratio2Threshold) {
            return (ch0CoeffRatio2 * ch0 - ch1CoeffRatio2 * ch1) / MEASUREMENT_COEFF;
        }
        else if (ratio < ratio3Threshold) {
            return (ch0CoeffRatio3 * ch0 + ch1CoeffRatio3 * ch1) / MEASUREMENT_COEFF;
        }

        return 0.0f;
    }

} // namespace bsp::light_sensor
