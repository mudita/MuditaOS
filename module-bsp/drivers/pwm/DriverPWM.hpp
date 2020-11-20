// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <sys/types.h>

namespace drivers
{

    enum class PWMInstances
    {
        PWM0,
        PWM1,
        PWM2,
        PWM3,
        COUNT
    };

    struct DriverPWMParams
    {
        uint32_t prescaler;
    };

    class DriverPWM
    {
      public:
        static std::shared_ptr<DriverPWM> Create(const PWMInstances inst, const DriverPWMParams &params);

        DriverPWM(const PWMInstances inst, const DriverPWMParams &params) : instance(inst), parameters(params)
        {}

        virtual ~DriverPWM()
        {}

        virtual void SetDutyCycle(uint32_t duty_cycle);

        virtual void Start();

        virtual void Stop();

      protected:
        PWMInstances instance;
        const DriverPWMParams parameters;

      private:
        static std::weak_ptr<DriverPWM> singleton[static_cast<uint32_t>(PWMInstances ::COUNT)];
    };

} // namespace drivers
