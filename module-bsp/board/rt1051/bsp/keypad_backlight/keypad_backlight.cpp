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

        static bool writeSingleRegister(uint32_t address, uint8_t *to_send)
        {
            addr.subAddress    = address;
            auto write_success = i2c->Write(addr, to_send, 1);
            if (write_success != 1) {
                return false;
            }
            return true;
        }

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
            bool status = reset();
            shutdown();

            if (status != 1) {

                return kStatus_Fail;
            }

            return kStatus_Success;
        }

        void deinit()
        {
            shutdown();
            qHandleIrq = NULL;
        }

        bool set(Diodes diode, DiodeIntensity intensity)
        {
            Diode_Reg diode_reg;
            diode_reg.max_current = MAX_DIODE_CURRENT_LIMIT;
            diode_reg.current     = encode_diode_brightness(intensity);

            switch (diode) {
            case Diodes::KEYPAD_LEFT:
                if (!writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::GREEN4),
                                         reinterpret_cast<uint8_t *>(&diode_reg)))
                    return false;
                break;
            case Diodes::KEYPAD_RIGHT:
                if (!writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::RED4),
                                         reinterpret_cast<uint8_t *>(&diode_reg)))
                    return false;
                break;
            default:
                return false;
                break;
            }
            return true;
        }

        bool set(Diodes diode, Rgb intensity)
        {
            Diode_Reg diode_reg;
            diode_reg.max_current = MAX_DIODE_CURRENT_LIMIT;

            switch (diode) {
            case Diodes::RGB_LEFT:
                diode_reg.current = encode_diode_brightness(intensity.red);
                if (!writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::RED3),
                                         reinterpret_cast<uint8_t *>(&diode_reg)))
                    return false;
                diode_reg.current = encode_diode_brightness(intensity.green);
                if (!writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::GREEN3),
                                         reinterpret_cast<uint8_t *>(&diode_reg)))
                    return false;
                diode_reg.current = encode_diode_brightness(intensity.blue);
                if (!writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::BLUE3),
                                         reinterpret_cast<uint8_t *>(&diode_reg)))
                    return false;
                break;
            case Diodes::RGB_RIGHT:
                diode_reg.current = encode_diode_brightness(intensity.red);
                if (!writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::RED2),
                                         reinterpret_cast<uint8_t *>(&diode_reg)))
                    return false;
                diode_reg.current = encode_diode_brightness(intensity.green);
                if (!writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::GREEN2),
                                         reinterpret_cast<uint8_t *>(&diode_reg)))
                    return false;
                diode_reg.current = encode_diode_brightness(intensity.blue);
                if (!writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::BLUE2),
                                         reinterpret_cast<uint8_t *>(&diode_reg)))
                    return false;
                break;
            default:
                return false;
                break;
            }
            return true;
        }

        bool turnOnAll()
        {
            wakeup();
            configureModule();
            return (set(Diodes::RGB_LEFT, {1.0f, 0.0f, 0.0f}) |  // Red
                    set(Diodes::RGB_RIGHT, {0.0f, 1.0f, 0.0f}) | // Green
                    set(Diodes::KEYPAD_LEFT, 1.0f) | set(Diodes::KEYPAD_RIGHT, 1.0f));
        }

        bool configureModule()
        {
            uint8_t reg_val = BOOST_OUTPUT_4V;
            if (!writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::BOOST_CTRL), &reg_val))
                return false;

            uint8_t reg_val = WAKEUP;
            if (!writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::ENABLES), &reg_val))
                return false;

            return true;
        }

        void shutdown()
        {
            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_NRST), 0);
        }

        void wakeup()
        {
            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::KEYPAD_BACKLIGHT_DRIVER_NRST), 1);
        }

        bool reset()
        {
            uint8_t reset_value = 0xff;
            return writeSingleRegister(static_cast<uint32_t>(LP55281_Registers::RESET), &reset_value);
        }
    } // namespace keypad_backlight
} // namespace bsp
