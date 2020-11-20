// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/pwm/DriverPWM.hpp"

#include "../fsl_drivers/fsl_common.h"
#include "../fsl_drivers/fsl_pwm.h"

#include "mutex.hpp"

namespace drivers
{

    class RT1051DriverPWM : public DriverPWM
    {
      public:
        RT1051DriverPWM(const PWMInstances inst, const DriverPWMParams &params);

        ~RT1051DriverPWM();

        void SetDutyCycle(uint32_t duty_cycle) override final;

        void Start() override final;

        void Stop() override final;

      private:
    };

} // namespace drivers
