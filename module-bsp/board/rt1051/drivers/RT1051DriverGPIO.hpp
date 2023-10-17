// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051DRIVERGPIO_HPP
#define PUREPHONE_RT1051DRIVERGPIO_HPP

#include "drivers/gpio/DriverGPIO.hpp"
#include "../common/fsl_drivers/fsl_gpio.h"
#include <string>

namespace drivers
{

    class RT1051DriverGPIO : public DriverGPIO
    {
      public:
        RT1051DriverGPIO(const GPIOInstances &inst, const DriverGPIOParams &params);

        ~RT1051DriverGPIO();

        // Configures specific pin
        int32_t ConfPin(const DriverGPIOPinParams &params);

        // Sets the output level of the multiple GPIO pins to the logic 1.
        void SetPort(const uint32_t mask) override final;

        // Sets the output level of the multiple GPIO pins to the logic 0.
        void ClearPort(const uint32_t mask) override final;

        // Reverses the current output logic of the multiple GPIO pins.
        void TogglePort(const uint32_t mask) override final;

        // Sets the output level of the individual GPIO pin to logic 1 or 0.
        void WritePin(const uint32_t pin, const uint8_t output) override final;

        // Reads the current input value of the GPIO port.
        uint8_t ReadPin(const uint32_t pin) override final;

        void EnableInterrupt(const uint32_t mask) override final;

        void DisableInterrupt(const uint32_t mask) override final;

        void ClearPortInterrupts(const uint32_t mask) override final;

      private:
        GPIOInstances instance;
        GPIO_Type *base;
        std::string name;
    };

} // namespace drivers

#endif // PUREPHONE_RT1051DRIVERGPIO_HPP
