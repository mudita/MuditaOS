// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TCA8418.hpp"

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <timers.h>
#include <bsp/keyboard/keyboard.hpp>
#include <board/BoardDefinitions.hpp>
#include <board.h>
#include <log/log.hpp>

#include <cstdint>

namespace bsp::keyboard
{
    namespace
    {
        using namespace drivers;

        enum class NotificationSource
        {
            regularKey     = 0x01,
            rightFnPress   = 0x02,
            rightFnRelease = 0x04,
        };

        enum class KeyboardPower
        {
            off = 0,
            on
        };

        constexpr auto keyboardContactOscillationTimeoutMS = 20;
        constexpr auto maxTransmissionRetry                = 3;
        constexpr auto chipResetMs                         = 1;
        constexpr auto chipRecoveryMs                      = 1;
        constexpr int retryMs[maxTransmissionRetry]        = {1, 3, 7};

        std::shared_ptr<DriverI2C> i2c;
        I2CAddress i2cAddr = {
            .deviceAddress = TCA8418_I2C_ADDRESS, .subAddress = 0, .subAddressSize = TCA8418_I2C_ADDRESS_SIZE};
        std::shared_ptr<DriverGPIO> gpio;

        xQueueHandle qHandleIrq                 = nullptr;
        TimerHandle_t rightFunctionalCheckTimer = nullptr;
        std::uint8_t rigthFunctionalLastState   = 0;

        constexpr std::uint8_t keyCountRegisterMask  = 0x0F;
        constexpr std::uint8_t keyCodeRegisterMask   = 0x7F;
        constexpr std::uint8_t eventTypeRegisterMask = 0x80;

        void rightFunctionalCheckTimerCallback(TimerHandle_t timer)
        {
            std::uint8_t right_functional_current_state =
                GPIO_PinRead(BOARD_KEYBOARD_RF_BUTTON_PORT, BOARD_KEYBOARD_RF_BUTTON_PIN);
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xTimerStop(timer, 0);

            // If user pressed button and it is not just the contact oscillation
            if (right_functional_current_state == rigthFunctionalLastState) {
                if (right_functional_current_state == 0) {
                    if (qHandleIrq != nullptr) {
                        std::uint8_t val = static_cast<std::uint8_t>(NotificationSource::rightFnPress);
                        xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
                    }
                }
                else {
                    if (qHandleIrq != nullptr) {
                        std::uint8_t val = static_cast<std::uint8_t>(NotificationSource::rightFnRelease);
                        xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
                    }
                }
            }

            gpio->EnableInterrupt(1U << static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_RF_BUTTON));
        }
    } // anonymous namespace

    void power(KeyboardPower state)
    {
        gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_RESET_PIN),
                       static_cast<std::uint8_t>(state));
    }

    void reset()
    {
        // According to datasheet, section 6.7
        power(KeyboardPower::off);
        // Reset pulse should be at least 120us
        vTaskDelay(pdMS_TO_TICKS(chipResetMs));
        power(KeyboardPower::on);
        // Recovery time is 120us
        vTaskDelay(pdMS_TO_TICKS(chipRecoveryMs));
    }

    bool write(const drivers::I2CAddress &addr, const std::uint8_t *txBuff, const std::size_t size)
    {
        auto ret = 0;
        for (auto i = 0; i < maxTransmissionRetry; ++i) {
            ret = i2c->Write(addr, txBuff, size);
            if (ret == size) {
                return true;
            }
            vTaskDelay(pdMS_TO_TICKS(retryMs[i]));
        }
        LOG_ERROR("Can't write data to keyboard. I2C error: %d", ret);
        return false;
    }

    bool read(const drivers::I2CAddress &addr, std::uint8_t *rxBuff, const std::size_t size)
    {
        auto ret = 0;
        for (auto i = 0; i < maxTransmissionRetry; ++i) {
            ret = i2c->Read(addr, rxBuff, size);
            if (ret == size) {
                return true;
            }
            vTaskDelay(pdMS_TO_TICKS(retryMs[i]));
        }
        LOG_ERROR("Can't read data from keyboard. I2C error: %d", ret);
        return false;
    }

    void clearAllIRQs()
    {
        std::uint8_t clearIRQs = 0xFF;
        i2cAddr.subAddress     = REG_INT_STAT;
        write(i2cAddr, reinterpret_cast<std::uint8_t *>(&clearIRQs), 1);
    }

    void configureRegisters()
    {
        std::uint32_t reg = 0;
        /* Assemble a mask for row and column registers */
        reg = ~(static_cast<std::uint32_t>(~0) << TCA8418_ROWS_COUNT);
        reg |= (~(static_cast<std::uint32_t>(~0) << TCA8418_COL_COUNT)) << 8;

        /* Set registers to keypad mode */
        i2cAddr.subAddress = REG_KP_GPIO1;
        write(i2cAddr, reinterpret_cast<std::uint8_t *>(&reg), 1);
        reg                = reg >> 8;
        i2cAddr.subAddress = REG_KP_GPIO2;
        write(i2cAddr, reinterpret_cast<std::uint8_t *>(&reg), 1);
        reg                = reg >> 16;
        i2cAddr.subAddress = REG_KP_GPIO3;
        write(i2cAddr, reinterpret_cast<std::uint8_t *>(&reg), 1);

        /* Enable column debouncing */
        i2cAddr.subAddress = REG_DEBOUNCE_DIS1;
        write(i2cAddr, reinterpret_cast<std::uint8_t *>(&reg), 1);
        reg                = reg >> 8;
        i2cAddr.subAddress = REG_DEBOUNCE_DIS2;
        write(i2cAddr, reinterpret_cast<std::uint8_t *>(&reg), 1);
        reg                = reg >> 16;
        i2cAddr.subAddress = REG_DEBOUNCE_DIS3;
        write(i2cAddr, reinterpret_cast<std::uint8_t *>(&reg), 1);

        reg = CFG_INT_CFG | CFG_OVR_FLOW_IEN | CFG_KE_IEN;
        // Enable interrupts
        i2cAddr.subAddress = REG_CFG;
        write(i2cAddr, reinterpret_cast<std::uint8_t *>(&reg), 1);

        // Get key pressed/released count
        std::uint8_t val   = 0;
        i2cAddr.subAddress = REG_KEY_LCK_EC;
        read(i2cAddr, reinterpret_cast<std::uint8_t *>(&val), 1);

        std::uint8_t keyCount = val & keyCountRegisterMask;
        for (auto i = 0; i < keyCount; i++) {
            i2cAddr.subAddress = REG_KEY_EVENT_A;
            read(i2cAddr, reinterpret_cast<std::uint8_t *>(&val), 1);
        }

        // Dummy read before clear IRQs
        std::uint8_t dummy = 0xFF;
        i2cAddr.subAddress = REG_INT_STAT;
        read(i2cAddr, reinterpret_cast<std::uint8_t *>(&dummy), 1);
        clearAllIRQs();
    }

    std::int32_t init(xQueueHandle qHandle)
    {
        i2c = DriverI2C::Create(
            static_cast<I2CInstances>(BoardDefinitions::KEYBOARD_I2C),
            DriverI2CParams{.baudrate = static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_I2C_BAUDRATE)});

        gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::KEYBOARD_GPIO), DriverGPIOParams{});

        qHandleIrq = qHandle;

        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                          .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                          .defLogic = 0,
                                          .pin = static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_RF_BUTTON)});

        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                          .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                          .defLogic = 0,
                                          .pin = static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_RESET_PIN)});

        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                          .irqMode  = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                                          .defLogic = 0,
                                          .pin      = static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_IRQ_PIN)});

        reset();
        configureRegisters();

        if (rightFunctionalCheckTimer == nullptr) {
            rightFunctionalCheckTimer = xTimerCreate("RfKeyTim",
                                                     pdMS_TO_TICKS(keyboardContactOscillationTimeoutMS),
                                                     false,
                                                     NULL,
                                                     rightFunctionalCheckTimerCallback);
            if (rightFunctionalCheckTimer == nullptr) {
                return kStatus_Fail;
            }
        }

        gpio->EnableInterrupt(1U << static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_RF_BUTTON));
        gpio->EnableInterrupt(1U << static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_IRQ_PIN));

        return kStatus_Success;
    }

    std::int32_t deinit()
    {

        gpio->DisableInterrupt(1U << static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_RF_BUTTON));
        gpio->DisableInterrupt(1U << static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_IRQ_PIN));

        gpio->ClearPortInterrupts(1U << static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_RF_BUTTON));
        gpio->ClearPortInterrupts(1U << static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_IRQ_PIN));

        clearAllIRQs();

        xTimerDelete(rightFunctionalCheckTimer, 50);
        rightFunctionalCheckTimer = nullptr;

        // Clear IRQ queue handle
        qHandleIrq = nullptr;

        i2c.reset();
        gpio.reset();

        return kStatus_Success;
    }

    std::vector<KeyEvent> getKeyEvents(KeyNotificationSource notification)
    {
        std::vector<KeyEvent> out;
        if (notification & static_cast<std::uint8_t>(NotificationSource::regularKey)) {
            uint8_t retval = 0;
            // Read how many key events has been registered
            i2cAddr.subAddress = REG_KEY_LCK_EC;
            read(i2cAddr, reinterpret_cast<std::uint8_t *>(&retval), 1);

            std::uint8_t eventsCount = retval & keyCountRegisterMask;
            for (std::uint8_t i = 0; i < eventsCount; i++) {
                std::uint8_t key      = 0;
                std::uint8_t rel_pres = 0;

                i2cAddr.subAddress = REG_KEY_EVENT_A;
                read(i2cAddr, reinterpret_cast<std::uint8_t *>(&retval), 1);

                key = retval & keyCodeRegisterMask;
                // key release/pressed is stored on the last bit
                rel_pres = (retval & eventTypeRegisterMask) >> 7;

                KeyEvent keyEvent{};
                keyEvent.code  = static_cast<KeyCodes>(key);
                keyEvent.event = static_cast<KeyEvents>(rel_pres);
                out.push_back(keyEvent);
            }

            clearAllIRQs();
        }

        if (notification & static_cast<std::uint8_t>(NotificationSource::rightFnPress)) {
            KeyEvent keyEvent{};
            keyEvent.code  = KeyCodes::FnRight;
            keyEvent.event = KeyEvents::Pressed;
            out.push_back(keyEvent);
        }

        if (notification & static_cast<std::uint8_t>(NotificationSource::rightFnRelease)) {
            KeyEvent keyEvent{};
            keyEvent.code  = KeyCodes::FnRight;
            keyEvent.event = KeyEvents::Released;
            out.push_back(keyEvent);
        }

        return out;
    }

    BaseType_t rightFunctionalIRQHandler()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        gpio->ClearPortInterrupts(1U << static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_RF_BUTTON));

        if (rightFunctionalCheckTimer != nullptr) {
            gpio->DisableInterrupt(1U << static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_RF_BUTTON));

            if (gpio->ReadPin(static_cast<std::uint32_t>(BoardDefinitions::KEYBOARD_RF_BUTTON)) == 0) {
                rigthFunctionalLastState = 0;
            }
            else {
                rigthFunctionalLastState = 1;
            }

            xTimerResetFromISR(rightFunctionalCheckTimer, &xHigherPriorityTaskWoken);
        }

        return xHigherPriorityTaskWoken;
    }

    BaseType_t IRQHandler()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (qHandleIrq != nullptr) {
            std::uint8_t val = static_cast<std::uint8_t>(NotificationSource::regularKey);
            xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
        }
        return xHigherPriorityTaskWoken;
    }
} // namespace bsp::keyboard
