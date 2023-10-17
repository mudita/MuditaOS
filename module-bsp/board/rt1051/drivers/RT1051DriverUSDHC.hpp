// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/usdhc/DriverUSDHC.hpp"
#include "RT1051DriverPLL2.hpp"
#include "board/clock_config.h"

namespace drivers
{
    class RT1051DriverUSDHC : public DriverUSDHC
    {
      public:
        explicit RT1051DriverUSDHC(std::string name, USDHCInstances inst);

        void Enable() final;
        void Disable() final;

      private:
        std::shared_ptr<RT1051DriverPLL2> pll2Driver;
        clock_ip_name_t usdhcClock;
    };

} // namespace drivers
