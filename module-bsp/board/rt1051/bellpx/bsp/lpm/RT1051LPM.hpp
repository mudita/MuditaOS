// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <bsp/lpm/RT1051LPMCommon.hpp>

namespace bsp
{
    class RT1051LPM : public RT1051LPMCommon
    {
      public:
        int32_t PowerOff() final;
        void EnableDcdcPowerSaveMode() final;
        void DisableDcdcPowerSaveMode() final;

        static void WatchdogIRQHandler();

      private:
        std::shared_ptr<drivers::DriverGPIO> gpio_wakeup;
    };

} // namespace bsp
