// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <cstdint>

namespace
{
    constexpr std::uint32_t RTC_CALIBRATION_VALUE = 0;
}

namespace bsp::rtc
{
    std::uint32_t getRtcCalibrationValue()
    {
        return RTC_CALIBRATION_VALUE;
    }
} // namespace bsp::rtc
