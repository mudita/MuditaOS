// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/keypad_backlight/keypad_backlight.hpp"
#include "LP55281.hpp"

#include "board/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"

#include "fsl_common.h"
#include <array>
#include <cmath>

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
            .deviceAddress = static_cast<std::uint32_t>(LP55281_DEVICE_ADDR), .subAddress = 0, .subAddressSize = 1};

        constexpr auto rgbChannelsNum = 3;

        constexpr std::array<LP55281_Registers, 8> usedChannels = {LP55281_Registers::RED2,
                                                                   LP55281_Registers::GREEN2,
                                                                   LP55281_Registers::BLUE2,
                                                                   LP55281_Registers::RED2,
                                                                   LP55281_Registers::GREEN2,
                                                                   LP55281_Registers::BLUE2,
                                                                   LP55281_Registers::RED4,
                                                                   LP55281_Registers::GREEN4};

        constexpr std::array<LP55281_Registers, 2> usedSingleOutputs = {LP55281_Registers::RED4,    // Keypad right side
                                                                        LP55281_Registers::GREEN4}; // Keypad left side

        constexpr auto gammaFactor = 2.8f;
        constexpr DiodeIntensity gammaCorrection(std::uint8_t brightness8bit)
        {
            float brightness = static_cast<float>(brightness8bit) / 255.0f;
            std::clamp(brightness, 0.0f, 1.0f);
            return std::pow(brightness, gammaFactor);
        }

        using SingleDiode = std::pair<LP55281_Registers, DiodeIntensity>;
        using RGBdiode    = std::array<SingleDiode, rgbChannelsNum>;

        // Channels intensity according to design specification
        constexpr RGBdiode rightRed = {
            std::make_pair(LP55281_Registers::RED2, gammaCorrection(255)), // Red right button
            std::make_pair(LP55281_Registers::GREEN2, gammaCorrection(68)),
            std::make_pair(LP55281_Registers::BLUE2, gammaCorrection(90))};

        constexpr RGBdiode leftGreen = {
            std::make_pair(LP55281_Registers::RED3, gammaCorrection(47)), // Green left button
            std::make_pair(LP55281_Registers::GREEN3, gammaCorrection(255)),
            std::make_pair(LP55281_Registers::BLUE3, gammaCorrection(137))};

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

        /* Explicitly release peripherals */
        i2c.reset();
        gpio.reset();
    }

    bool configureDiodes()
    {
        constexpr DiodeIntensity intensity = 1.0f; // Maximum brightness
        Diode_Reg diodeReg                 = {.max_current = MAX_DIODE_CURRENT_LIMIT,
                              .current     = encode_diode_brightness_to_6bits(intensity)};

        for (const auto &diode : usedSingleOutputs) {
            const auto address = static_cast<std::uint32_t>(diode);
            if (!writeSingleRegister(address, reinterpret_cast<std::uint8_t *>(&diodeReg))) {
                return false;
            }
        }

        std::vector<const RGBdiode *> rgbDiodes = {&rightRed, &leftGreen};
        for (const auto &diodes : rgbDiodes) {
            for (const auto &diode : *diodes) {
                const auto address = static_cast<std::uint32_t>(diode.first);
                diodeReg.current   = encode_diode_brightness_to_6bits(diode.second);
                if (!writeSingleRegister(address, reinterpret_cast<std::uint8_t *>(&diodeReg))) {
                    return false;
                }
            }
        }
        return true;
    }

    bool turnOnAll()
    {
        wakeup();
        configureModule(ALL_PORTS_EN);
        return configureDiodes();
    }

    bool turnOnFunctionKeysBacklight()
    {
        wakeup();
        configureModule(FUNCTION_KEYS_PORTS_EN);
        return configureDiodes();
    }

    bool turnOnNumericKeysBacklight()
    {
        wakeup();
        configureModule(NUMERIC_KEYS_PORT_EN);
        return configureDiodes();
    }

    bool configureModule(std::uint8_t enablesRegisterValue)
    {
        std::uint8_t boostControlRegisterValue = BOOST_OUTPUT_4V;
        if (!writeSingleRegister(static_cast<std::uint32_t>(LP55281_Registers::BOOST_CTRL),
                                 &boostControlRegisterValue)) {
            return false;
        }

        if (!writeSingleRegister(static_cast<std::uint32_t>(LP55281_Registers::ENABLES), &enablesRegisterValue)) {
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
        for (const auto diode : usedChannels) {
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
