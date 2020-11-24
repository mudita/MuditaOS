// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/keypad_backlight/keypad_backlight.hpp"
#include "LP55281.hpp"

#include "bsp/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"

#include "fsl_common.h"

using namespace drivers;

static std::shared_ptr<drivers::DriverI2C> i2c;

namespace bsp
{

    namespace keypad_backlight
    {
        static xQueueHandle qHandleIrq = NULL;

        static I2CAddress addr = {.deviceAddress = static_cast<uint32_t>(LP55281_DEVICE_ADDR), .subAddressSize = 1};

        std::shared_ptr<DriverGPIO> gpio;

        int32_t init(xQueueHandle qHandle)
        {
            i2c = DriverI2C::Create(static_cast<I2CInstances>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_I2C),
                                    DriverI2CParams{.baudrate = static_cast<uint32_t>(
                                                        BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_I2C_BAUDRATE)});

            qHandleIrq = qHandle;

            gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_GPIO),
                                      DriverGPIOParams{});

            // OUTPUT
            gpio->ConfPin(
                DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                    .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                    .defLogic = 1,
                                    .pin      = static_cast<uint32_t>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_NRST)});

            wakeup();
            addr.subAddress     = static_cast<uint32_t>(LP55281_Addresses::RESET);
            uint8_t reset_value = 0xff;
            auto write_success  = i2c->Write(addr, (uint8_t *)(&reset_value), 1);
            shutdown();

            if (write_success != 1) {
                return kStatus_Fail;
            }

            return kStatus_Success;
        }

        void deinit()
        {
            shutdown();
            qHandleIrq = NULL;
        }

        bool set(Diode diode, DiodeIntensity intensity)
        {
            addr.subAddress = static_cast<uint32_t>(LP55281_Addresses::RED4);

            uint8_t diode_reg = 0xff; // encode_diode_brightness(intensity)|MAX_DIODE_CURRENT_LIMIT;

            auto wrote = i2c->Write(addr, (uint8_t *)(&diode_reg), 1);
            if (wrote != 1) {
                return false;
            }

            addr.subAddress = static_cast<uint32_t>(LP55281_Addresses::GREEN4);

            diode_reg = 0xff; // encode_diode_brightness(intensity)|MAX_DIODE_CURRENT_LIMIT;

            wrote = i2c->Write(addr, (uint8_t *)(&diode_reg), 1);
            if (wrote != 1) {
                return false;
            }

            addr.subAddress = static_cast<uint32_t>(LP55281_Addresses::GREEN2);

            diode_reg = 0xff; // encode_diode_brightness(intensity)|MAX_DIODE_CURRENT_LIMIT;

            wrote = i2c->Write(addr, (uint8_t *)(&diode_reg), 1);
            if (wrote != 1) {
                return false;
            }

            addr.subAddress = static_cast<uint32_t>(LP55281_Addresses::RED3);

            diode_reg = 0xff; // encode_diode_brightness(intensity)|MAX_DIODE_CURRENT_LIMIT;

            wrote = i2c->Write(addr, (uint8_t *)(&diode_reg), 1);
            if (wrote != 1) {
                return false;
            }

            addr.subAddress = static_cast<uint32_t>(LP55281_Addresses::BOOST_CTRL);
            diode_reg       = BOOST_OUTPUT_4V;

            wrote = i2c->Write(addr, (uint8_t *)(&diode_reg), 1);
            if (wrote != 1) {
                return false;
            }

            addr.subAddress = static_cast<uint32_t>(LP55281_Addresses::ENABLES);
            diode_reg       = WAKEUP;

            wrote = i2c->Write(addr, (uint8_t *)(&diode_reg), 1);
            if (wrote != 1) {
                return false;
            }

            return true;
        }

        bool set(Diode diode, Rgb intensity)
        {
            return false;
        }

        bool toggle(Diode diode)
        {
            if (!on) {
                on = true;
                wakeup();
                return bsp::keypad_backlight::set(bsp::keypad_backlight::Diode::KEYPAD_ALL, 1.0f);
            }
            else {
                shutdown();
                on = false;
                return true;
            }
        }

        void shutdown()
        {
            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_NRST), 0);
        }

        void wakeup()
        {
            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_NRST), 1);
        }

        void reset()
        {
            addr.subAddress     = static_cast<uint32_t>(LP55281_Addresses::RESET);
            uint8_t reset_value = 0xff;
            i2c->Write(addr, (uint8_t *)(&diode_reg), 1);
        }
    } // namespace keypad_backlight
} // namespace bsp
