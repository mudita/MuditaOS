#include "vibrator.hpp"

#include <bsp/BoardDefinitions.hpp>
#include <drivers/gpio/DriverGPIO.hpp>

static std::shared_ptr<drivers::DriverGPIO> port;

namespace bsp
{
    namespace vibrator
    {

        using namespace drivers;

        void disable();
        void init()
        {
            port = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::VIBRATOR_GPIO), DriverGPIOParams{});
            port->ConfPin(DriverGPIOPinParams{.dir = DriverGPIOPinParams::Direction::Output,
                                              .irqMode = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                              .defLogic = 0,
                                              .pin = static_cast<uint32_t>(BoardDefinitions::VIBRATOR_EN)});

            disable();
        }
        void enable()
        {
            port->WritePin(static_cast<uint32_t>(BoardDefinitions::VIBRATOR_EN), 1);
        }
        void disable()
        {
            port->WritePin(static_cast<uint32_t>(BoardDefinitions::VIBRATOR_EN), 0);
        }
    } // namespace vibrator
} // namespace bsp
