// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/light_sensor/light_sensor.hpp"
#include "LTR303ALS.hpp"
#include "board/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"
#include "drivers/gpio/DriverGPIO.hpp"

#include "fsl_common.h"
#include <log.hpp>

namespace bsp::light_sensor
{
    namespace
    {
        std::shared_ptr<drivers::DriverGPIO> gpio;
        std::shared_ptr<drivers::DriverI2C> i2c;
        xQueueHandle qHandleIrq = nullptr;

        drivers::I2CAddress addr = {
            .deviceAddress = static_cast<uint32_t>(LTR303ALS_DEVICE_ADDR), .subAddress = 0, .subAddressSize = 1};

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
            return i2c->Read(addr, readout, 4);
        }

        constexpr inline std::uint16_t irqLevelUp  = 0xffff;
        constexpr inline std::uint16_t irqLevelLow = 0;

        void configureInterrupts() __attribute__((used));
        void configureInterrupts()
        {
            // Has to be done before sensor active mode
            std::uint8_t reg = ENABLE_IRQ;
            writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::INTERRUPT), &reg);
            reg = irqLevelUp & 0xff;
            writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_THRES_UP_0), &reg);
            reg = irqLevelUp >> 8;
            writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_THRES_UP_1), &reg);
            reg = irqLevelLow & 0xff;
            writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_THRES_LOW_0), &reg);
            reg = irqLevelLow >> 8;
            writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_THRES_LOW_1), &reg);
        }

        void configureMeasurement()
        {
            std::uint8_t reg = MEAS_SETUP;
            writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_MEAS_RATE), &reg);
        }

    } // namespace

    std::int32_t init(xQueueHandle qHandle)
    {
        qHandleIrq = qHandle;

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
        // Uncomment to enable interrupts
        // gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::LIGHT_SENSOR_IRQ));

        reset();
        configureMeasurement();
        // Uncomment to enable interrupts
        // configureInterrupts();
        wakeup();

        return isPresent() ? kStatus_Success : kStatus_Fail;
    }

    void deinit()
    {
        qHandleIrq = nullptr;
        reset();
    }

    bool standby()
    {
        uint8_t reg = 0;
        return writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_CONTR), &reg);
    }

    bool wakeup()
    {
        std::uint8_t reg = ACTIVE_MODE;
        return writeSingleRegister(static_cast<std::uint32_t>(LTR303ALS_Registers::ALS_CONTR), &reg);
    }

    IlluminanceLux readout()
    {
        uint8_t reg[4];
        readMeasurementRegisters(reg);
        return decodeLightMeasurement(reg);
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
        return readout == MANUFACTURER_ID;
    }

    BaseType_t IRQHandler()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (qHandleIrq != nullptr) {
            std::uint8_t val = 0x01;
            xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
        }
        return xHigherPriorityTaskWoken;
    }

} // namespace bsp::light_sensor
