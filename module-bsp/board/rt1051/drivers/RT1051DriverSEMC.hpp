// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/semc/DriverSEMC.hpp"
#include "RT1051DriverPLL2.hpp"

namespace drivers
{
    class RT1051DriverSEMC : public DriverSEMC
    {
      public:
        explicit RT1051DriverSEMC(std::string name);

        void Enable() final;
        void Disable() final;

        void SwitchToPLL2ClockSource() final;
        void SwitchToPeripheralClockSource() final;

      private:
        std::shared_ptr<RT1051DriverPLL2> pll2Driver;
    };

} // namespace drivers
