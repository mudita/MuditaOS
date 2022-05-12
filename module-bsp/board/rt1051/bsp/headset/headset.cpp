// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "headset.hpp"

#include "board/BoardDefinitions.hpp"
#include "DriverI2C.hpp"
#include "DriverGPIO.hpp"
#include "fsl_common.h"
#include "timers.h"

#include <log/log.hpp>

namespace bsp
{
    namespace headset
    {
        namespace
        {
            using namespace drivers;

            constexpr uint8_t HEADSET_I2C_ADDR = 0x3B;

            constexpr uint8_t HEADSET_INTERRUPT_REG      = 0x01;
            constexpr uint8_t HEADSET_KEY_PRESS_INT_REG  = 0x02;
            constexpr uint8_t HEADSET_INTERRUPT_DIS_REG  = 0x03;
            constexpr uint8_t HEADSET_DEV_SETTINGS_REG   = 0x04;
            constexpr uint8_t HEADSET_DEV_SETTING_1_REG  = 0x05;
            constexpr uint8_t HEADSET_ACCESSORY_STAT_REG = 0x0B;

            constexpr uint8_t HEADSET_DET_THRESH1_ADDR = 0x0D;
            constexpr uint8_t HEADSET_DET_THRESH2_ADDR = 0x0E;
            constexpr uint8_t HEADSET_DET_THRESH3_ADDR = 0x0F;

            constexpr uint8_t HEADSET_INT_DIS_INT_DIS = 1 << 3;
            constexpr uint8_t HEADSET_INT_DIS_INT_ENA = 0 << 3;
            constexpr uint8_t HEADSET_INT_DIS_ADC_DIS = 1 << 2;
            constexpr uint8_t HEADSET_INT_DIS_ADC_ENA = 0 << 2;
            constexpr uint8_t HEADSET_INT_DIS_DC_DIS  = 1 << 1;
            constexpr uint8_t HEADSET_INT_DIS_DC_ENA  = 0 << 1;
            constexpr uint8_t HEADSET_INT_DIS_INS_DIS = 1 << 0;
            constexpr uint8_t HEADSET_INT_DIS_INS_ENA = 0 << 0;

            constexpr uint8_t HEADSET_DET_THRESH1_VAL = 0x22;
            constexpr uint8_t HEADSET_DET_THRESH2_VAL = 0x23;
            constexpr uint8_t HEADSET_DET_THRESH3_VAL = 0x6C;

            constexpr uint8_t HEADSET_DEV_SET_DET_TRIG = 1 << 4;
            constexpr uint8_t HEADSET_DEV_SET_DET_EN   = 1 << 5;
            constexpr uint8_t HEADSET_DEV_SET_RESET    = 1 << 7;
            constexpr uint8_t HEADSET_DEV_SET_DEB_1S   = 0x06;

            constexpr uint8_t HEADSET_DEV_SET1_KP_EN = 1 << 2;

            constexpr uint16_t HEADSET_POLL_INTERVAL_MS = 500;

            enum class HeadsetIQRDetectionResult : uint8_t
            {
                DETRES_3POLE            = 1 << 0,
                DETRES_OMTP             = 1 << 1,
                DETRES_CTIA             = 1 << 2,
                DETRES_INSERTION_STATUS = 1 << 3
            };

            enum class HeadsetIQRKeyPressStatus : uint8_t
            {
                KEY4_RELEASE = 1 << 7,
                KEY4_PRESS   = 1 << 6,
                KEY3_RELEASE = 1 << 5,
                KEY3_PRESS   = 1 << 4,
                KEY2_RELEASE = 1 << 3,
                KEY2_PRESS   = 1 << 2,
                KEY1_RELEASE = 1 << 1,
                KEY1_PRESS   = 1 << 0,
            };

            enum class HeadsetType : uint8_t
            {
                Type3Pole,
                TypeOMTP,
                TypeCTIA,
                TypeDontCare
            };

        } // anonymous namespace

        static std::shared_ptr<drivers::DriverI2C> i2c;
        static std::shared_ptr<drivers::DriverGPIO> gpio;
        static drivers::I2CAddress i2cAddr = {
            .deviceAddress = HEADSET_I2C_ADDR, .subAddress = 0, .subAddressSize = 0x01};
        static TimerHandle_t timerHandle;

        static xQueueHandle qHandleIrq = nullptr;
        static bool HeadsetInserted    = false;
        static bool MicrophoneInserted = false;

        static HeadsetType insertedHeadsetType = HeadsetType::TypeDontCare;

        static void readKeyCodeAndEvent(uint8_t &keyEvent, uint8_t &keyCode)
        {
            uint8_t keypress_int = 0;

            i2cAddr.subAddress = HEADSET_KEY_PRESS_INT_REG;
            i2c->Read(i2cAddr, static_cast<uint8_t *>(&keypress_int), 1);

            switch (static_cast<HeadsetIQRKeyPressStatus>(keypress_int)) {
            case HeadsetIQRKeyPressStatus::KEY4_RELEASE:
                keyCode  = static_cast<uint8_t>(KeyCode::Key4);
                keyEvent = static_cast<uint8_t>(KeyEvent::KeyReleased);
                break;
            case HeadsetIQRKeyPressStatus::KEY4_PRESS:
                keyCode  = static_cast<uint8_t>(KeyCode::Key4);
                keyEvent = static_cast<uint8_t>(KeyEvent::KeyPressed);
                break;
            case HeadsetIQRKeyPressStatus::KEY3_RELEASE:
                keyCode  = static_cast<uint8_t>(KeyCode::Key3);
                keyEvent = static_cast<uint8_t>(KeyEvent::KeyReleased);
                break;
            case HeadsetIQRKeyPressStatus::KEY3_PRESS:
                keyCode  = static_cast<uint8_t>(KeyCode::Key3);
                keyEvent = static_cast<uint8_t>(KeyEvent::KeyPressed);
                break;
            case HeadsetIQRKeyPressStatus::KEY2_RELEASE:
                keyCode  = static_cast<uint8_t>(KeyCode::Key2);
                keyEvent = static_cast<uint8_t>(KeyEvent::KeyReleased);
                break;
            case HeadsetIQRKeyPressStatus::KEY2_PRESS:
                keyCode  = static_cast<uint8_t>(KeyCode::Key2);
                keyEvent = static_cast<uint8_t>(KeyEvent::KeyPressed);
                break;
            case HeadsetIQRKeyPressStatus::KEY1_RELEASE:
                keyCode  = static_cast<uint8_t>(KeyCode::Key1);
                keyEvent = static_cast<uint8_t>(KeyEvent::KeyReleased);
                break;
            case HeadsetIQRKeyPressStatus::KEY1_PRESS:
                keyCode  = static_cast<uint8_t>(KeyCode::Key1);
                keyEvent = static_cast<uint8_t>(KeyEvent::KeyPressed);
                break;
            default:
                keyCode  = static_cast<uint8_t>(KeyCode::Error);
                keyEvent = static_cast<uint8_t>(KeyEvent::Error);
                break;
            }
        }

        HeadsetState headset_get_data(bool &headsetState, bool &microphoneState, uint8_t &keyEvent, uint8_t &keyCode)
        {
            uint8_t reg;
            uint8_t acc_status              = 0;
            HeadsetState headsetStateChange = HeadsetState::NoChange;

            // Clear event flags
            i2cAddr.subAddress = HEADSET_INTERRUPT_REG;
            i2c->Read(i2cAddr, static_cast<uint8_t *>(&reg), 1);

            i2cAddr.subAddress = HEADSET_ACCESSORY_STAT_REG;
            i2c->Read(i2cAddr, static_cast<uint8_t *>(&acc_status), 1);

            // Check if jack removed event
            if (((acc_status & static_cast<uint8_t>(HeadsetIQRDetectionResult::DETRES_INSERTION_STATUS)) == 0) &&
                HeadsetInserted) {
                HeadsetInserted = false;
                MicrophoneInserted = false;

                LOG_INFO("Headset removed");

                // Turn off key press/release detection
                reg                = HEADSET_DEV_SET1_KP_EN;
                i2cAddr.subAddress = HEADSET_DEV_SETTING_1_REG;
                i2c->Modify(i2cAddr, reg, MicrophoneInserted, 1);

                headsetState       = HeadsetInserted;
                microphoneState    = MicrophoneInserted;
                headsetStateChange = HeadsetState::Changed;
            }
            else if (((acc_status & static_cast<uint8_t>(HeadsetIQRDetectionResult::DETRES_INSERTION_STATUS)) != 0) &&
                     !HeadsetInserted) {
                LOG_INFO("Headset inserted");
                HeadsetInserted = true;

                acc_status &= ~static_cast<uint8_t>(HeadsetIQRDetectionResult::DETRES_INSERTION_STATUS);

                switch (static_cast<HeadsetIQRDetectionResult>(acc_status)) {
                case HeadsetIQRDetectionResult::DETRES_3POLE:
                    LOG_INFO("Headset 3-pole detected");
                    MicrophoneInserted = false;
                    insertedHeadsetType = HeadsetType::Type3Pole;
                    break;
                case HeadsetIQRDetectionResult::DETRES_OMTP:
                    LOG_INFO("Headset 4-pole OMTP detected");
                    MicrophoneInserted = true;
                    insertedHeadsetType = HeadsetType::TypeOMTP;
                    break;
                case HeadsetIQRDetectionResult::DETRES_CTIA:
                    LOG_INFO("Headset 4-pole Standard detected");
                    MicrophoneInserted = true;
                    insertedHeadsetType = HeadsetType::TypeCTIA;
                    break;
                default:
                    LOG_INFO("Unknown Headset type detected");
                    MicrophoneInserted  = false;
                    insertedHeadsetType = HeadsetType::TypeDontCare;
                }

                // Turn on/off key press/release detection
                reg                = HEADSET_DEV_SET1_KP_EN;
                i2cAddr.subAddress = HEADSET_DEV_SETTING_1_REG;
                i2c->Modify(i2cAddr, reg, MicrophoneInserted, 1);

                headsetState       = HeadsetInserted;
                microphoneState    = MicrophoneInserted;
                headsetStateChange = HeadsetState::Changed;
            }
            // Key pressed/released event
            else {
                readKeyCodeAndEvent(keyEvent, keyCode);
            }

            return headsetStateChange;
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
            // Headset jack external GPIO pin configuration
            gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::MIC_BIAS_DRIVER_GPIO),
                                      DriverGPIOParams{});

            gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::MIC_BIAS_DRIVER_EN)});

            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::MIC_BIAS_DRIVER_EN), 1);

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

            // Reset headset controller
            uint8_t reg        = HEADSET_DEV_SET_RESET;
            i2cAddr.subAddress = HEADSET_DEV_SETTINGS_REG;
            i2c->Write(i2cAddr, static_cast<uint8_t *>(&reg), 1);

            qHandleIrq = qHandle;

            HeadsetInserted = false;
            MicrophoneInserted = false;

            // Set Insertion de-bounce time to 1s, enable auto-detection and manually trigger detection
            reg                = HEADSET_DEV_SET_DEB_1S | HEADSET_DEV_SET_DET_EN | HEADSET_DEV_SET_DET_TRIG;
            i2cAddr.subAddress = HEADSET_DEV_SETTINGS_REG;
            i2c->Write(i2cAddr, static_cast<uint8_t *>(&reg), 1);

            // Turn off DC and ADC conversion interrupt
            reg = HEADSET_INT_DIS_INT_ENA | HEADSET_INT_DIS_ADC_DIS | HEADSET_INT_DIS_DC_DIS | HEADSET_INT_DIS_INS_ENA;

            i2cAddr.subAddress = HEADSET_INTERRUPT_DIS_REG;
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

        bool IsInserted()
        {
            return HeadsetInserted;
        }

        status_t Deinit()
        {
            qHandleIrq      = nullptr;
            HeadsetInserted = false;
            MicrophoneInserted = false;

            uint8_t reg        = HEADSET_DEV_SET_RESET;
            i2cAddr.subAddress = HEADSET_DEV_SETTINGS_REG;
            i2c->Write(i2cAddr, static_cast<uint8_t *>(&reg), 1);
            i2c.reset();

            gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::MIC_BIAS_DRIVER_EN), 0);
            gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::HEADSET_IRQ_PIN));

            return kStatus_Success;
        }
    } // namespace headset
} // namespace bsp
