// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bell_temp_sensor/bell_temp_sensor.hpp"
#include "CT7117.hpp"
#include <board/BoardDefinitions.hpp>
#include "drivers/i2c/DriverI2C.hpp"
#include "drivers/gpio/DriverGPIO.hpp"

#include "fsl_common.h"
#include <log/log.hpp>

namespace bsp::bell_temp_sensor
{
    bool isFahrenheit = false;

    namespace
    {
        std::shared_ptr<drivers::DriverI2C> i2c;
        bool isInitiated = false;

        drivers::I2CAddress addr = {
            .deviceAddress = static_cast<uint32_t>(CT7117_DEVICE_ADDR), .subAddress = 0, .subAddressSize = 1};

        bool writeSingleRegister(std::uint32_t address, std::uint16_t *to_send)
        {
            addr.subAddress          = address;
            const auto write_success = i2c->Write(addr, reinterpret_cast<uint8_t *>(to_send), 2);

            return write_success == 1;
        }

        ssize_t readSingleRegister(std::uint32_t address, std::uint16_t *readout)
        {
            addr.subAddress = address;
            return i2c->Read(addr, reinterpret_cast<uint8_t *>(readout), 2);
        }

        ssize_t readMeasurementRegisters(std::uint16_t *readout)
        {
            addr.subAddress = static_cast<std::uint32_t>(CT7117_Registers::Temp);
            return i2c->Read(addr, reinterpret_cast<uint8_t *>(readout), 2);
        }

    } // namespace

    std::int32_t init(bool Fahrenheit)
    {
        isFahrenheit = Fahrenheit;

        LOG_DEBUG("Initializing Bell temperature sensor");

        if (isInitiated) {
            return isPresent() ? kStatus_Success : kStatus_Fail;
        }

        drivers::DriverI2CParams i2cParams;
        i2cParams.baudrate = static_cast<std::uint32_t>(BoardDefinitions::BELL_TEMP_SENSOR_I2C_BAUDRATE);
        i2c = drivers::DriverI2C::Create(static_cast<drivers::I2CInstances>(BoardDefinitions::BELL_TEMP_SENSOR_I2C),
                                         i2cParams);

        wakeup();

        isInitiated = true;

        return isPresent() ? kStatus_Success : kStatus_Fail;
    }

    void deinit()
    {
        standby();
    }

    bool standby()
    {
        uint16_t reg = 0;
        readSingleRegister(static_cast<uint32_t>(CT7117_Registers::Config), &reg);
        reg |= static_cast<uint16_t>(CT7117_Config_Reg::SD);
        return writeSingleRegister(static_cast<std::uint32_t>(CT7117_Registers::Config), &reg);
    }

    bool wakeup()
    {
        uint16_t reg = 0;
        readSingleRegister(static_cast<uint32_t>(CT7117_Registers::Config), &reg);
        reg &= ~(static_cast<uint16_t>(CT7117_Config_Reg::SD));
        return writeSingleRegister(static_cast<std::uint32_t>(CT7117_Registers::Config), &reg);
    }

    Temperature readout()
    {
        uint8_t reg[2] = {0, 0};
        float temp     = 0.0;

        readSingleRegister(static_cast<uint32_t>(CT7117_Registers::Temp), reinterpret_cast<uint16_t *>(&reg[0]));
        uint16_t reg16 =
            (static_cast<uint16_t>(reg[0]) << 8) | (static_cast<uint16_t>(reg[1]) & 0xFFE0); // 0.25 C resolution

        uint16_t integer    = 0;
        uint16_t fractional = 0;

        if (reg16 & 0x8000) {                                                 // sign bit present
            integer    = static_cast<uint16_t>(((~reg16 + 1) & 0x7FFF) >> 7); // remove sign bit and shift to lower byte
            fractional = static_cast<uint16_t>(((~reg16 + 1) & 0x7F) * 0.78125);
            temp       = -1 * (static_cast<float>(integer) + (static_cast<float>(fractional) / 100.0));
        }
        else {
            integer    = static_cast<uint16_t>((reg16 & 0x7FFF) >> 7); // remove sign bit and shift to lower byte
            fractional = static_cast<uint16_t>((reg16 & 0x7F) * 0.78125);
            temp       = static_cast<float>(integer) + (static_cast<float>(fractional) / 100.0);
        }

        if (isFahrenheit)
            temp = (temp * 1.8) + 32.00;

        return temp;
    }

    bool isPresent()
    {
        std::uint8_t readout;
        addr.subAddress = static_cast<uint8_t>(CT7117_Registers::ID);
        i2c->Read(addr, reinterpret_cast<uint8_t *>(&readout), 1);

        LOG_DEBUG("Bell temperature sensor %s", (readout == CT7117_DEVICE_ID) ? "present" : "error !");

        return readout == CT7117_DEVICE_ID;
    }

} // namespace bsp::bell_temp_sensor
