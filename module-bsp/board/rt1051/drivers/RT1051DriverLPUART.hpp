// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/lpuart/DriverLPUART.hpp"
#include "RT1051DriverOscillator.hpp"
#include "board/clock_config.h"

namespace drivers
{
    class RT1051DriverLPUART : public DriverLPUART
    {
      public:
        explicit RT1051DriverLPUART(std::string name, LPUARTInstances inst);

        void Enable() final;
        void Disable() final;

      private:
        std::shared_ptr<RT1051DriverOscillator> oscillatorDriver;
        clock_ip_name_t lpuartClock;
    };

} // namespace drivers
