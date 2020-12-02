// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/light_sensor/light_sensor.hpp"
#include "LTR303ALS.hpp"
#include "bsp/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"
#include "drivers/gpio/DriverGPIO.hpp"

#include "fsl_common.h"
#include "log/log.hpp"

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

namespace bsp::light_sensor
{
    namespace
    {
        std::shared_ptr<drivers::DriverGPIO> gpio;
        std::shared_ptr<drivers::DriverI2C> i2c;

        drivers::I2CAddress addr = {.deviceAddress = static_cast<uint32_t>(LTR303ALS_DEVICE_ADDR), .subAddressSize = 1};

        bool writeSingleRegister(std::uint32_t address, std::uint8_t *to_send)
        {
            addr.subAddress    = address;
            auto write_success = i2c->Write(addr, to_send, 1);

            return write_success == 1;
        }

        ssize_t readSingleRegister(std::uint32_t address, std::uint8_t *readout)
        {
            addr.subAddress = address;
            return i2c->Read(addr, readout, 1);
        }

        ssize_t readMeasurementRegisters(std::uint8_t *readout)
        {
            addr.subAddress = static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_DATA_CH1_0);
            return i2c->Read(addr, readout, 4);
        }

    } // namespace

    std::int32_t init()
    {
        drivers::DriverI2CParams i2cParams;
        i2cParams.baudrate = static_cast<std::uint32_t>(BoardDefinitions::LIGHT_SENSOR_I2C_BAUDRATE);
        i2c = drivers::DriverI2C::Create(static_cast<drivers::I2CInstances>(BoardDefinitions::LIGHT_SENSOR_I2C),
                                         i2cParams);

        gpio = drivers::DriverGPIO::Create(static_cast<drivers::GPIOInstances>(BoardDefinitions::LIGHT_SENSOR_IRQ_GPIO),
                                           drivers::DriverGPIOParams{});

        drivers::DriverGPIOPinParams gpioParams;
        gpioParams.dir     = drivers::DriverGPIOPinParams::Direction::Input;
        gpioParams.irqMode = drivers::DriverGPIOPinParams::InterruptMode::IntFallingEdge;
        gpioParams.pin     = static_cast<uint32_t>(BoardDefinitions::LIGHT_SENSOR_IRQ);
        gpio->ConfPin(gpioParams);
        gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::LIGHT_SENSOR_IRQ));

        wakeup();

        return isPresent() ? kStatus_Success : kStatus_Fail;
    }

    void deinit()
    {}

    bool standby()
    {
        return reset();
    }

    void wakeup()
    {
        uint8_t reg = ACTIVE_MODE;
        writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_CONTR), &reg);
    }

    void readout()
    {
        uint8_t reg[4];
        readMeasurementRegisters(reg);
        float readout = decodeVisibleLightMeasurement(reg);

        LOG_DEBUG("!!! Light measurement: %d", static_cast<int>(readout));
    }

    bool reset()
    {
        uint8_t reg = SW_RESET;
        return writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_CONTR), &reg);
    }

    bool isPresent()
    {
        std::uint8_t readout;
        readSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::MANUFAC_ID), &readout);
        if (readout != MANUFACTURER_ID) {
            return false;
        }
        return true;
    }

} // namespace bsp::light_sensor
