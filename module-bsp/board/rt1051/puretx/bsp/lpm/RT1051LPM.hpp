// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/lpm/RT1051LPMCommon.hpp>

namespace bsp
{
    class RT1051LPM : public RT1051LPMCommon
    {
      public:
        RT1051LPM();
        void EnableDcdcPowerSaveMode() final;
        void DisableDcdcPowerSaveMode() final;

        void AllowEnteringWfiMode() final;
        void BlockEnteringWfiMode() final;

        std::uint32_t EnterWfiModeIfAllowed() final;
        std::uint32_t GetLastTimeSpentInWfi() final;

        void DisableSysTick() final;
        void EnableSysTick() final;

        std::uint32_t DisableInterrupts() final;
        void EnableInterrupts(std::uint32_t primask) final;

      private:
        std::shared_ptr<drivers::DriverGPIO> gpio_1;
        std::shared_ptr<drivers::DriverGPIO> gpio_2;
    };
} // namespace bsp
