// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/watchdog/watchdog.hpp"
#include <fsl_rtwdog.h>
#include <limits>
#include <cstdint>

namespace bsp::watchdog
{
    bool init(unsigned timeoutMs)
    {
        // 32.768kHz source clock divided by 256 prescaler
        constexpr unsigned clockFreqHz = 32768 / 256;

        const unsigned timeoutValueTicks = timeoutMs * clockFreqHz / 1000;
        if (timeoutValueTicks > std::numeric_limits<std::uint16_t>::max()) {
            return false;
        }

        rtwdog_config_t config;
        RTWDOG_GetDefaultConfig(&config);
        config.enableRtwdog         = true;
        config.clockSource          = kRTWDOG_ClockSource1;            // LPO_CLK clock (32.768kHz)
        config.prescaler            = kRTWDOG_ClockPrescalerDivide256; // 256 prescaler (effectively 128Hz clock)
        config.workMode.enableWait  = false;
        config.workMode.enableStop  = false;
        config.workMode.enableDebug = false; // If true, RTWDOG will run when target is halted
        config.testMode             = kRTWDOG_TestModeDisabled;
        config.enableUpdate         = true;
        config.enableInterrupt      = true;
        config.enableWindowMode     = false;
        config.windowValue          = 0;
        config.timeoutValue         = static_cast<std::uint16_t>(timeoutValueTicks);
        RTWDOG_Init(RTWDOG, &config);

        return true;
    }

    void refresh()
    {
        RTWDOG_Refresh(RTWDOG);
    }
} // namespace bsp::watchdog
