// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "headset.hpp"

#include "BoardDefinitions.hpp"
#include "DriverI2C.hpp"
#include "DriverGPIO.hpp"
#include "fsl_common.h"
#include "timers.h"

namespace bsp
{
    namespace headset
    {
        using namespace drivers;

        static constexpr uint8_t HEADSET_I2C_ADDR     = 0x3B;
        static constexpr uint8_t HEADSET_INT_REG_ADDR = 0x01;
        static constexpr uint8_t HEADSET_INT_DIS_ADDR = 0x03;
        static constexpr uint8_t HEADSET_DEV_SET_ADDR = 0x04;
        static constexpr uint8_t HEADSET_DET_RES_ADDR = 0x0B;

        static constexpr uint8_t HEADSET_DET_THRESH1_ADDR = 0x0D;
        static constexpr uint8_t HEADSET_DET_THRESH2_ADDR = 0x0E;
        static constexpr uint8_t HEADSET_DET_THRESH3_ADDR = 0x0F;

        static constexpr uint8_t HEADSET_INT_DIS_INT_DIS = 1 << 3;
        static constexpr uint8_t HEADSET_INT_DIS_INT_ENA = 0 << 3;
        static constexpr uint8_t HEADSET_INT_DIS_ADC_DIS = 1 << 2;
        static constexpr uint8_t HEADSET_INT_DIS_ADC_ENA = 0 << 2;
        static constexpr uint8_t HEADSET_INT_DIS_DC_DIS  = 1 << 1;
        static constexpr uint8_t HEADSET_INT_DIS_DC_ENA  = 0 << 1;
        static constexpr uint8_t HEADSET_INT_DIS_INS_DIS = 1 << 0;
        static constexpr uint8_t HEADSET_INT_DIS_INS_ENA = 0 << 0;

        static constexpr uint8_t HEADSET_DET_THRESH1_VAL = 0x22;
        static constexpr uint8_t HEADSET_DET_THRESH2_VAL = 0x23;
        static constexpr uint8_t HEADSET_DET_THRESH3_VAL = 0x6C;

        static constexpr uint8_t HEADSET_DEV_SET_DET_EN = 1 << 5;
        static constexpr uint8_t HEADSET_DEV_SET_DEB_1S = 0x06;

        static constexpr uint16_t HEADSET_POLL_INTERVAL_MS = 500;

        static std::shared_ptr<drivers::DriverI2C> i2c;
        static std::shared_ptr<drivers::DriverGPIO> gpio;
        static drivers::I2CAddress i2cAddr = {
            .deviceAddress = HEADSET_I2C_ADDR, .subAddress = 0, .subAddressSize = 0x01};
        static TimerHandle_t timerHandle;

        static xQueueHandle qHandleIrq = nullptr;
        static bool HeadsetInserted    = false;
        static bool MicrophoneInserted = false;

        static bool ReadInsertionStatus()
        {
            uint8_t reg;
            bool ret = false;

            i2cAddr.subAddress = HEADSET_INT_REG_ADDR;
            i2c->Read(i2cAddr, (uint8_t *)&reg, 1);

            i2cAddr.subAddress = HEADSET_DET_RES_ADDR;
            i2c->Read(i2cAddr, (uint8_t *)&reg, 1);

            if (((reg & 0x08) == 0) && (HeadsetInserted == true)) {
                HeadsetInserted = false;
                MicrophoneInserted = false;
                LOG_INFO("Headset removed");
                gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::MIC_BIAS_DRIVER_EN), 0);

                ret = true;
            }

            if (((reg & 0x08) != 0) && (HeadsetInserted == false)) {
                LOG_INFO("Headset inserted");
                HeadsetInserted = true;

                if ((reg & 0x01) != 0) {
                    LOG_INFO("Headset 3-pole detected");
                    MicrophoneInserted = false;
                }
                if ((reg & 0x02) != 0) {
                    LOG_INFO("Headset 4-pole OMTP detected");
                    MicrophoneInserted = true;
                }
                if ((reg & 0x04) != 0) {
                    LOG_INFO("Headset 4-pole Standard detected");
                    MicrophoneInserted = true;
                }
                ret = true;
            }

            if (MicrophoneInserted == true) {
                gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::MIC_BIAS_DRIVER_EN), 1);
            }
            else {
                gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::MIC_BIAS_DRIVER_EN), 0);
            }

            return ret;
        }

        static void TimerHandler(TimerHandle_t xTimer)
        {
            if (qHandleIrq != nullptr) {
                uint8_t val = 0x01;
                xQueueSend(qHandleIrq, &val, 0);
                xTimerDelete(xTimer, 0);
            }
        }

        BaseType_t headset_IRQHandler()
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (qHandleIrq != nullptr) {
                uint8_t val = 0x01;
                xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
            }
            return xHigherPriorityTaskWoken;
        }

        status_t Init(xQueueHandle qHandle)
        {
            // Microphone jack external GPIO pin configuration
            gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::MIC_BIAS_DRIVER_GPIO),
                                      DriverGPIOParams{});

            gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::MIC_BIAS_DRIVER_EN)});

            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::MIC_BIAS_DRIVER_EN), 0);

            // Jack detection I2C and GPIO configuration

            // Init input JACKDET IRQ
            gpio->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::HEADSET_IRQ_PIN));
            gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::HEADSET_IRQ_PIN));

            gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::HEADSET_IRQ_PIN)});

            // Enable GPIO pin interrupt
            gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::HEADSET_IRQ_PIN));

            i2c = DriverI2C::Create(
                static_cast<I2CInstances>(BoardDefinitions::HEADSET_I2C),
                DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::HEADSET_I2C_BAUDRATE)});

            qHandleIrq = qHandle;

            HeadsetInserted = false;
            MicrophoneInserted = false;

            // Turn off DC and ADC conversion interrupt
            uint8_t reg =
                HEADSET_INT_DIS_INT_ENA | HEADSET_INT_DIS_ADC_ENA | HEADSET_INT_DIS_DC_ENA | HEADSET_INT_DIS_INS_ENA;

            i2cAddr.subAddress = HEADSET_INT_DIS_ADDR;
            i2c->Write(i2cAddr, static_cast<uint8_t *>(&reg), 1);

            // Set Insertion de-bounce time to 1s, enable auto-detection
            reg                = HEADSET_DEV_SET_DET_EN | HEADSET_DEV_SET_DEB_1S;
            i2cAddr.subAddress = HEADSET_DEV_SET_ADDR;
            i2c->Write(i2cAddr, static_cast<uint8_t *>(&reg), 1);

            // Set detection thresholds
            reg                = HEADSET_DET_THRESH1_VAL;
            i2cAddr.subAddress = HEADSET_DET_THRESH1_ADDR;
            i2c->Write(i2cAddr, static_cast<uint8_t *>(&reg), 1);

            reg                = HEADSET_DET_THRESH2_VAL;
            i2cAddr.subAddress = HEADSET_DET_THRESH2_ADDR;
            i2c->Write(i2cAddr, static_cast<uint8_t *>(&reg), 1);

            reg                = HEADSET_DET_THRESH3_VAL;
            i2cAddr.subAddress = HEADSET_DET_THRESH3_ADDR;
            i2c->Write(i2cAddr, static_cast<uint8_t *>(&reg), 1);

            if (timerHandle == nullptr) {
                timerHandle =
                    xTimerCreate("HeadsetTimer", pdMS_TO_TICKS(HEADSET_POLL_INTERVAL_MS), false, nullptr, TimerHandler);
                if (timerHandle == nullptr) {
                    LOG_FATAL("Could not create the timer for Headset insertion/removal detection");
                    return kStatus_Fail;
                }
            }

            xTimerStart(timerHandle, HEADSET_POLL_INTERVAL_MS);

            return kStatus_Success;
        }

        bool Handler(uint8_t notification)
        {
            if (notification == 0x01) {
                return ReadInsertionStatus();
            }

            return false;
        }

        bool IsInserted()
        {
            return HeadsetInserted;
        }

        status_t Deinit()
        {
            qHandleIrq      = nullptr;
            HeadsetInserted = false;
            MicrophoneInserted = false;

            i2c.reset();

            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::MIC_BIAS_DRIVER_EN), 0);

            return kStatus_Success;
        }
    } // namespace headset
} // namespace bsp
