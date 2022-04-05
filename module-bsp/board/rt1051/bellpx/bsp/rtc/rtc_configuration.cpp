// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/rtc/rtc.hpp>

namespace
{
    static constexpr uint32_t RTC_CALIBRATION_VALUE = 0;
}

namespace bsp::rtc
{
    uint32_t getRtcCalibrationValue()
    {
        return RTC_CALIBRATION_VALUE;
    }
} // namespace bsp::rtc
