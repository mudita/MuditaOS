// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <sys/types.h>

namespace drivers
{

    enum class PWMInstances
    {
        PWM_1,
        PWM_2,
        PWM_3,
        PWM_4,
        COUNT
    };

    enum class PWMModules
    {
        MODULE0,
        MODULE1,
        MODULE2,
        MODULE3,
        COUNT
    };

    enum class PWMChannel
    {
        A,
        B,
        X
    };
    struct DriverPWMParams
    {
        PWMChannel channel;
        uint32_t prescaler;
    };

    class DriverPWM
    {
      public:
        static std::shared_ptr<DriverPWM> Create(const PWMInstances inst,
                                                 const PWMModules mod,
                                                 const DriverPWMParams &params);

        DriverPWM(const PWMInstances inst, const PWMModules mod, const DriverPWMParams &params)
            : instance(inst), module(mod), parameters(params)
        {}

        virtual ~DriverPWM()
        {}

        // Duty cycle in percent: 0 - 100
        virtual void SetDutyCycle(uint8_t duty_cycle);

        virtual void Start();

        virtual void Stop();

      protected:
        PWMInstances instance;
        PWMModules module;
        const DriverPWMParams parameters;

      private:
        static std::weak_ptr<DriverPWM> singleton[static_cast<uint32_t>(PWMInstances::COUNT)]
                                                 [static_cast<uint32_t>(PWMModules::COUNT)];
    };

} // namespace drivers
