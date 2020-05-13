
#include "bsp/magnetometer/magnetometer.hpp"

#include "bsp/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"

using namespace drivers;

std::shared_ptr<drivers::DriverI2C> i2cDev;
static drivers::I2CAddress addr = {.deviceAddress = 0x64, .subAddressSize = 1};

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{

    namespace magnetometer
    {

        int32_t init(xQueueHandle qHandle)
        {
            i2cDev = DriverI2C::Create(
                static_cast<I2CInstances>(BoardDefinitions::MAGNETOMETER_I2C),
                DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_I2C_BAUDRATE)});

            qHandleIrq = qHandle;
            return 1;
        }

        bool isPresent(void)
        {
            uint8_t buf;
            addr.subAddress = 0x00;
            auto readed     = i2cDev->Read(addr, &buf, 1);

            if (readed != 1) {
                return false;
            }
            return true;
        }

        bsp::Board GetBoard(void)
        {
            if (isPresent()) {
                return bsp::Board::T4;
            }
            return bsp::Board::T3;
        }
    } // namespace magnetometer
} // namespace bsp
