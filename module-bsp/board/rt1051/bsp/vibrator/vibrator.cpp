// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/vibrator/vibrator.hpp"

#include <board/BoardDefinitions.hpp>
#include <drivers/gpio/DriverGPIO.hpp>

static std::shared_ptr<drivers::DriverGPIO> port;

namespace bsp
{
    namespace vibrator
    {

        using namespace drivers;

        void enable()
        {
            port->WritePin(static_cast<uint32_t>(BoardDefinitions::VIBRATOR_EN), 1);
        }
        void disable()
        {
            port->WritePin(static_cast<uint32_t>(BoardDefinitions::VIBRATOR_EN), 0);
        }

        void init(std::chrono::milliseconds pulse)
        {
            port = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::VIBRATOR_GPIO), DriverGPIOParams{});
            port->ConfPin(DriverGPIOPinParams{.dir = DriverGPIOPinParams::Direction::Output,
                                              .irqMode = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                              .defLogic = 0,
                                              .pin = static_cast<uint32_t>(BoardDefinitions::VIBRATOR_EN)});

            disable();
        }

        void deinit()
        {
            disable();
        }

    } // namespace vibrator
} // namespace bsp
