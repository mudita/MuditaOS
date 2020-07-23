
#include "bsp/torch/torch.hpp"

#include "bsp/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"

#include "fsl_common.h"

using namespace drivers;

static std::shared_ptr<drivers::DriverI2C> i2c;
static drivers::I2CAddress addr = {.deviceAddress = 0x63, .subAddressSize = 1};

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{

    namespace torch
    {
        std::shared_ptr<DriverGPIO> gpio;

        int32_t init(xQueueHandle qHandle)
        {
            i2c = DriverI2C::Create(
                static_cast<I2CInstances>(BoardDefinitions::TORCH_DRIVER_I2C),
                DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::TORCH_DRIVER_I2C_BAUDRATE)});

            qHandleIrq = qHandle;

            gpio =
                DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::TORCH_DRIVER_GPIO), DriverGPIOParams{});

            // OUTPUT
            gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::TORCH_DRIVER_EN)});
            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions ::TORCH_DRIVER_EN), 1);
            return kStatus_Success;
        }

        bool isPresent(void)
        {
            uint8_t buf;
            addr.subAddress = 0x0C;
            auto read       = i2c->Read(addr, &buf, 1);

            if (read != 1) {
                return false;
            }

            if (buf == 0b100 || buf == 0b010) {
                return true;
            }
            LOG_WARN("Something is present at the torch LED driver address (0x%lx), but not the AL3644TT",
                     addr.deviceAddress);
            return false;
        }
    } // namespace torch
} // namespace bsp
