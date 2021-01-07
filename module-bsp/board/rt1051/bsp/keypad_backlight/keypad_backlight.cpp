// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/keypad_backlight/keypad_backlight.hpp"
#include "LP55281.hpp"

#include "bsp/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"

#include "fsl_common.h"
#include <array>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

namespace bsp::keypad_backlight
{
    namespace
    {
        std::shared_ptr<drivers::DriverGPIO> gpio;

        std::shared_ptr<drivers::DriverI2C> i2c;

        drivers::I2CAddress addr = {
            .deviceAddress = static_cast<uint32_t>(LP55281_DEVICE_ADDR), .subAddress = 0, .subAddressSize = 1};

        constexpr std::array<LP55281_Registers, 4> usedOutputs = {LP55281_Registers::RED2,    // Red right button
                                                                  LP55281_Registers::GREEN3,  // Green left button
                                                                  LP55281_Registers::RED4,    // Keypad right side
                                                                  LP55281_Registers::GREEN4}; // Keypad left side

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
    } // namespace

    std::int32_t init()
    {
        i2c = drivers::DriverI2C::Create(
            static_cast<drivers::I2CInstances>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_I2C),
            drivers::DriverI2CParams{
                .baudrate = static_cast<std::uint32_t>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_I2C_BAUDRATE)});

        gpio = drivers::DriverGPIO::Create(
            static_cast<drivers::GPIOInstances>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_GPIO),
            drivers::DriverGPIOParams{});

        gpio->ConfPin(drivers::DriverGPIOPinParams{
            .dir      = drivers::DriverGPIOPinParams::Direction::Output,
            .irqMode  = drivers::DriverGPIOPinParams::InterruptMode::NoIntmode,
            .defLogic = 1,
            .pin      = static_cast<std::uint32_t>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_NRST)});

        wakeup();
        bool status = reset();
        shutdown();

        return status ? kStatus_Success : kStatus_Fail;
    }

    void deinit()
    {
        shutdown();
    }

    bool turnOnAll()
    {
        constexpr DiodeIntensity intensity = 1.0f; // Maximum brightness
        Diode_Reg diode_reg                = {.max_current = MAX_DIODE_CURRENT_LIMIT,
                               .current     = encode_diode_brightness_to_6bits(intensity)};

        wakeup();
        configureModule();

        for (auto &diode : usedOutputs) {
            std::uint32_t address = static_cast<std::uint32_t>(diode);
            if (!writeSingleRegister(address, reinterpret_cast<std::uint8_t *>(&diode_reg))) {
                return false;
            }
        }
        return true;
    }

    bool configureModule()
    {
        std::uint8_t reg_val = BOOST_OUTPUT_4V;
        if (!writeSingleRegister(static_cast<std::uint32_t>(LP55281_Registers::BOOST_CTRL), &reg_val)) {
            return false;
        }

        reg_val = WAKEUP;
        if (!writeSingleRegister(static_cast<std::uint32_t>(LP55281_Registers::ENABLES), &reg_val)) {
            return false;
        }

        return true;
    }

    bool shutdown()
    {
        gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_NRST), 0);
        return true;
    }

    void wakeup()
    {
        gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_NRST), 1);
    }

    bool reset()
    {
        std::uint8_t reset_value = 0xff;
        return writeSingleRegister(static_cast<std::uint32_t>(LP55281_Registers::RESET), &reset_value);
    }

    // Must be run at least 20ms after leds startup
    bool checkState()
    {
        std::uint8_t value = 0;
        for (const auto diode : usedOutputs) {
            value = static_cast<std::uint8_t>(diode) | EN_LED_TEST;

            if (!writeSingleRegister(static_cast<std::uint32_t>(LP55281_Registers::LED_TEST), &value)) {
                return false;
            }

            vTaskDelay(pdMS_TO_TICKS(2));

            if (readSingleRegister(static_cast<std::uint32_t>(LP55281_Registers::ADC_OUT), &value) != 1) {
                return false;
            }

            if (value < LED_TEST_THRESHOLD) {
                return false;
            }
        }

        value = 0;
        writeSingleRegister(static_cast<std::uint32_t>(LP55281_Registers::LED_TEST), &value);

        return true;
    }

} // namespace bsp::keypad_backlight
