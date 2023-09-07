// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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

      private:
        std::shared_ptr<drivers::DriverGPIO> gpio_1;
        std::shared_ptr<drivers::DriverGPIO> gpio_2;
    };
} // namespace bsp
