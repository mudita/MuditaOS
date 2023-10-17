// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/light_sensor/light_sensor.hpp"
#include "LTR303ALS.hpp"
#include "board/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"

#include "fsl_common.h"
#include <log/log.hpp>

namespace bsp::light_sensor
{
    namespace
    {
        constexpr auto measurementRegsSize = 4; // bytes

        std::shared_ptr<drivers::DriverI2C> i2c;

        drivers::I2CAddress addr = {
            .deviceAddress = static_cast<std::uint32_t>(LTR303ALS_DEVICE_ADDR), .subAddress = 0, .subAddressSize = 1};

        bool writeSingleRegister(std::uint32_t address, std::uint8_t *to_send)
        {
            addr.subAddress          = address;
            const auto write_success = i2c->Write(addr, to_send, 1);

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
            return i2c->Read(addr, readout, measurementRegsSize);
        }

        void configureMeasurement()
        {
            std::uint8_t reg = MEAS_SETUP;
            writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_MEAS_RATE), &reg);
        }

    } // namespace

    std::int32_t init()
    {
        drivers::DriverI2CParams i2cParams{};
        i2cParams.baudrate = static_cast<std::uint32_t>(BoardDefinitions::LIGHT_SENSOR_I2C_BAUDRATE);
        i2c = drivers::DriverI2C::Create(static_cast<drivers::I2CInstances>(BoardDefinitions::LIGHT_SENSOR_I2C),
                                         i2cParams);

        reset();
        configureMeasurement();
        wakeup();

        return isPresent() ? kStatus_Success : kStatus_Fail;
    }

    void deinit()
    {
        reset();
        /* Explicitly release I2C peripheral */
        i2c.reset();
    }

    bool standby()
    {
        std::uint8_t reg = 0;
        return writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_CONTR), &reg);
    }

    bool wakeup()
    {
        std::uint8_t reg = ACTIVE_MODE;
        return writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_CONTR), &reg);
    }

    IlluminanceLux readout()
    {
        std::uint8_t reg[measurementRegsSize];
        readMeasurementRegisters(reg);
        return decodeLightMeasurement(reg);
    }

    bool reset()
    {
        std::uint8_t reg = SW_RESET;
        return writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_CONTR), &reg);
    }

    bool isPresent()
    {
        std::uint8_t readout;
        readSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::MANUFAC_ID), &readout);
        return readout == MANUFACTURER_ID;
    }
} // namespace bsp::light_sensor
