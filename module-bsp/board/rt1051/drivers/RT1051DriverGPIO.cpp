// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverGPIO.hpp"
#include <log/log.hpp>

namespace drivers
{

    RT1051DriverGPIO::RT1051DriverGPIO(const GPIOInstances &inst, const DriverGPIOParams &params)
        : DriverGPIO(params), instance(inst)
    {
        switch (instance) {
        case GPIOInstances::GPIO_1: {
            base = GPIO1;
            name = "GPIO_1";
        } break;

        case GPIOInstances::GPIO_2: {
            base = GPIO2;
            name = "GPIO_2";
        } break;

        case GPIOInstances::GPIO_3: {
            base = GPIO3;
            name = "GPIO_3";
        } break;

        case GPIOInstances::GPIO_4: {
            base = GPIO4;
            name = "GPIO_4";
        } break;

        case GPIOInstances::GPIO_5: {
            base = GPIO5;
            name = "GPIO_5";
        } break;
        default:
            break;
        }

        LOG_DEBUG("Init: %s", name.c_str());
    }

    RT1051DriverGPIO::~RT1051DriverGPIO()
    {
        LOG_DEBUG("Deinit: %s", name.c_str());
        // Disable all interrupts
        GPIO_DisableInterrupts(base, UINT32_MAX);
        // Clear all interrupt flags
        GPIO_ClearPinsInterruptFlags(base, UINT32_MAX);
        GPIO_Deinit(base);
    }

    int32_t RT1051DriverGPIO::ConfPin(const drivers::DriverGPIOPinParams &params)
    {
        gpio_pin_config_t conf;
        conf.direction     = static_cast<gpio_pin_direction_t>(params.dir);
        conf.outputLogic   = static_cast<uint8_t>(params.defLogic);
        conf.interruptMode = static_cast<gpio_interrupt_mode_t>(params.irqMode);
        GPIO_PinInit(base, params.pin, &conf);
        return 0;
    }

    void RT1051DriverGPIO::SetPort(const uint32_t mask)
    {
        GPIO_PortSet(base, mask);
    }

    void RT1051DriverGPIO::ClearPort(const uint32_t mask)
    {
        GPIO_PortClear(base, mask);
    }

    void RT1051DriverGPIO::TogglePort(const uint32_t mask)
    {
        GPIO_PortToggle(base, mask);
    }

    void RT1051DriverGPIO::WritePin(const uint32_t pin, const uint8_t output)
    {
        GPIO_PinWrite(base, pin, output);
    }

    uint8_t RT1051DriverGPIO::ReadPin(const uint32_t pin)
    {
        return GPIO_PinRead(base, pin);
    }

    void RT1051DriverGPIO::EnableInterrupt(const uint32_t mask)
    {
        GPIO_EnableInterrupts(base, mask);
    }

    void RT1051DriverGPIO::DisableInterrupt(const uint32_t mask)
    {
        GPIO_DisableInterrupts(base, mask);
    }

    void RT1051DriverGPIO::ClearPortInterrupts(const uint32_t mask)
    {
        GPIO_PortClearInterruptFlags(base, mask);
    }

} // namespace drivers
