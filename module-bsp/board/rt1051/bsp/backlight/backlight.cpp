// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/backlight/backlight.hpp"
#include "LP55281.hpp"

#include "bsp/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"

#include "fsl_common.h"

using namespace drivers;

static std::shared_ptr<drivers::DriverI2C> i2c;

namespace bsp
{

    namespace backlight
    {
        static xQueueHandle qHandleIrq = NULL;

        static I2CAddress addr = {.deviceAddress = LP55281_DEVICE_ADDR, .subAddressSize = 1};

        std::shared_ptr<DriverGPIO> gpio;

        int32_t init(xQueueHandle qHandle)
        {
            i2c = DriverI2C::Create(
                static_cast<I2CInstances>(BoardDefinitions::BACKLIGHT_DRIVER_I2C_BAUDRATE),
                DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::BACKLIGHT_DRIVER_I2C_BAUDRATE)});

            qHandleIrq = qHandle;

            gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BACKLIGHT_DRIVER_GPIO),
                                      DriverGPIOParams{});

            // OUTPUT
            gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                              .defLogic = 0,
                                              .pin = static_cast<uint32_t>(BoardDefinitions::BACKLIGHT_DRIVER_NRST)});

            return kStatus_Success;
        }

        void deinit()
        {
            qHandleIrq = NULL;
        }

        bool set(Diode diode, DiodeIntensity intensity)
        {
            return false;
        }

        bool set(Diode diode, Rgb intensity)
        {
            return false;
        }

        bool toggle(Diode diode)
        {
            return false;
        }

        void backlight_shutdown()
        {}

        void backlight_wakeup()
        {}

        void backlight_reset()
        {}
    } // namespace backlight
} // namespace bsp
