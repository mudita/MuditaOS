// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Utils.hpp> // for byte conversion functions. it is included first because of magic enum define

#include <FreeRTOS.h>
#include <mutex.hpp>
#include <queue.h>
#include <task.h>
#include <timers.h>
#include <bsp/switches/switches.hpp>
#include <board/BoardDefinitions.hpp>
#include <board.h>
#include <fsl_common.h>

#include <chrono>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <map>
#include <vector>
#include <magic_enum.hpp>

using namespace drivers;
using namespace utils;

namespace bsp::bell_switches
{
    using namespace std::chrono_literals;

    using TimerCallback = void (*)(TimerHandle_t timerHandle);

    constexpr std::chrono::milliseconds contactOscillationTimeout{30ms};
    constexpr std::chrono::milliseconds centerKeyPressValidationTimeout{30ms};

    enum class DebounceTimerId : unsigned int
    {
        leftSideSwitch = 0,
        rightSideSwitch,
        lightCenterSwitch,
        latchSwitch,
        wakeup,
        Invalid
    };

    void debounceTimerCallback(TimerHandle_t timerHandle);

    struct DebounceTimerState
    {
        const DebounceTimerId id{DebounceTimerId::Invalid};
        const NotificationSource notificationSource{NotificationSource::Invalid};
        const std::shared_ptr<drivers::DriverGPIO> gpio;
        const BoardDefinitions pin{-1};
        KeyEvents lastState{KeyEvents::Pressed};
        TimerHandle_t timer{nullptr};

        void createTimer(TimerCallback callback, const std::chrono::milliseconds timeout)
        {
            timer =
                xTimerCreate(magic_enum::enum_name(id).data(), pdMS_TO_TICKS(timeout.count()), false, this, callback);
        }
    };

    static struct LatchEventFlag
    {
      private:
        static constexpr std::chrono::milliseconds latchPressEventTimeout = 1000ms;
        cpp_freertos::MutexStandard latchFlagMutex;
        std::chrono::time_point<std::chrono::system_clock> timeOfLastLatchEvent = std::chrono::system_clock::now();
        bool pressed                                                            = false;

      public:
        void setPressed()
        {
            cpp_freertos::LockGuard lock(latchFlagMutex);
            pressed              = true;
            timeOfLastLatchEvent = std::chrono::system_clock::now();
        }

        void setReleased()
        {
            cpp_freertos::LockGuard lock(latchFlagMutex);
            pressed              = false;
            timeOfLastLatchEvent = std::chrono::system_clock::now();
        }

        bool isPressed()
        {
            cpp_freertos::LockGuard lock(latchFlagMutex);
            return pressed;
        }

        bool wasJustPressed()
        {
            cpp_freertos::LockGuard lock(latchFlagMutex);
            auto ret = ((std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now() - timeOfLastLatchEvent)) <= latchPressEventTimeout);
            return ret;
        }

    } latchEventFlag;

    static std::map<DebounceTimerId, DebounceTimerState> debounceTimers;

    static xQueueHandle qHandleIrq{};
    std::shared_ptr<DriverGPIO> gpio_sw;
    std::shared_ptr<DriverGPIO> gpio_wakeup;

    void debounceTimerCallback(TimerHandle_t timerHandle)
    {
        auto timerState                     = static_cast<DebounceTimerState *>(pvTimerGetTimerID(timerHandle));
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerStop(timerState->timer, 0);

        auto currentState = timerState->gpio->ReadPin(static_cast<uint32_t>(timerState->pin)) ? KeyEvents::Released
                                                                                              : KeyEvents::Pressed;

        if (currentState == timerState->lastState && qHandleIrq != nullptr) {
            if (currentState == KeyEvents::Pressed) {
                if (timerState->notificationSource == NotificationSource::latchKeyPress) {
                    latchEventFlag.setPressed();
                }
                auto val = static_cast<std::uint16_t>(timerState->notificationSource);
                xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
                timerState->lastState = KeyEvents::Released;
            }
            else {
                if (timerState->notificationSource == NotificationSource::latchKeyPress) {
                    latchEventFlag.setReleased();
                }
                auto val = NotificationSource::Invalid;
                switch (timerState->notificationSource) {
                case NotificationSource::leftSideKeyPress:
                    val = NotificationSource::leftSideKeyRelease;
                    break;
                case NotificationSource::rightSideKeyPress:
                    val = NotificationSource::rightSideKeyRelease;
                    break;
                case NotificationSource::lightCenterKeyPress:
                    val = NotificationSource::lightCenterKeyRelease;
                    break;
                case NotificationSource::latchKeyPress:
                    val = NotificationSource::latchKeyRelease;
                    break;
                case NotificationSource::wakeupEvent:
                    val = NotificationSource::wakeupEventRelease;
                    break;
                default:
                    break;
                }
                xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
                timerState->lastState = KeyEvents::Pressed;
            }
        }

        timerState->gpio->EnableInterrupt(1U << static_cast<uint32_t>(timerState->pin));
    }

    void debounceTimerCenterClickCallback(TimerHandle_t timerHandle)
    {
        auto timerState                     = static_cast<DebounceTimerState *>(pvTimerGetTimerID(timerHandle));
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerStop(timerState->timer, 0);

        if (!latchEventFlag.wasJustPressed() && qHandleIrq != nullptr) {
            auto val = static_cast<std::uint16_t>(timerState->notificationSource);
            xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
        }

        timerState->gpio->ClearPortInterrupts(1U << static_cast<uint32_t>(timerState->pin));
        timerState->gpio->EnableInterrupt(1U << static_cast<uint32_t>(timerState->pin));
    }

    KeyEvents getLatchState()
    {
        return (gpio_sw->ReadPin(static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH)) ? KeyEvents::Released
                                                                                               : KeyEvents::Pressed);
    }

    void addDebounceTimer(DebounceTimerState timerState)
    {
        debounceTimers.insert({timerState.id, timerState});
        DebounceTimerState &state = debounceTimers.find(timerState.id)->second;

        if (timerState.notificationSource == NotificationSource::lightCenterKeyPress) {
            state.createTimer(debounceTimerCenterClickCallback, centerKeyPressValidationTimeout);
        }
        else if (timerState.notificationSource == NotificationSource::latchKeyPress) {
            state.createTimer(debounceTimerCallback, contactOscillationTimeout);
            state.lastState = getLatchState();
            if (state.lastState == KeyEvents::Released) {
                latchEventFlag.setReleased();
            }
            else {
                latchEventFlag.setPressed();
            }
        }
        else {
            state.createTimer(debounceTimerCallback, contactOscillationTimeout);
        }
    }

    void configureSwitch(BoardDefinitions boardSwitch, DriverGPIOPinParams::InterruptMode mode)
    {

        gpio_sw->ClearPortInterrupts(1 << magic_enum::enum_integer(boardSwitch));
        gpio_sw->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                             .irqMode  = mode,
                                             .defLogic = 1,
                                             .pin      = static_cast<uint32_t>(boardSwitch)});
    }

    int32_t init(xQueueHandle qHandle)
    {
        qHandleIrq = qHandle;

        // Switches
        gpio_sw =
            DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_SWITCHES_GPIO), DriverGPIOParams{});
        // wakeup
        gpio_wakeup =
            DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_WAKEUP_GPIO), DriverGPIOParams{});

        configureSwitch(BoardDefinitions::BELL_SWITCHES_LATCH,
                        DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge);
        configureSwitch(BoardDefinitions::BELL_SWITCHES_LEFT,
                        DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge);
        configureSwitch(BoardDefinitions::BELL_SWITCHES_RIGHT,
                        DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge);
        configureSwitch(BoardDefinitions::BELL_SWITCHES_CENTER, DriverGPIOPinParams::InterruptMode::IntRisingEdge);
        configureSwitch(BoardDefinitions::BELL_WAKEUP, DriverGPIOPinParams::InterruptMode::IntFallingEdge);

        addDebounceTimer(DebounceTimerState{DebounceTimerId::leftSideSwitch,
                                            NotificationSource::leftSideKeyPress,
                                            gpio_sw,
                                            BoardDefinitions::BELL_SWITCHES_LEFT});
        addDebounceTimer(DebounceTimerState{DebounceTimerId::rightSideSwitch,
                                            NotificationSource::rightSideKeyPress,
                                            gpio_sw,
                                            BoardDefinitions::BELL_SWITCHES_RIGHT});
        addDebounceTimer(DebounceTimerState{DebounceTimerId::lightCenterSwitch,
                                            NotificationSource::lightCenterKeyPress,
                                            gpio_sw,
                                            BoardDefinitions::BELL_SWITCHES_CENTER});
        addDebounceTimer(DebounceTimerState{DebounceTimerId::latchSwitch,
                                            NotificationSource::latchKeyPress,
                                            gpio_sw,
                                            BoardDefinitions::BELL_SWITCHES_LATCH,
                                            KeyEvents::Released});
        addDebounceTimer(DebounceTimerState{
            DebounceTimerId::wakeup, NotificationSource::wakeupEvent, gpio_wakeup, BoardDefinitions::BELL_WAKEUP});

        enableIRQ();

        return kStatus_Success;
    }

    std::int32_t deinit()
    {
        qHandleIrq = nullptr;
        disableIRQ();

        for (const auto &el : debounceTimers) {
            xTimerDelete(el.second.timer, 50);
        }
        debounceTimers.clear();

        gpio_sw.reset();
        gpio_wakeup.reset();

        return kStatus_Success;
    }

    void getDebounceTimer(BaseType_t xHigherPriorityTaskWoken, DebounceTimerId timerId)
    {
        if (debounceTimers.find(timerId) == debounceTimers.end()) {
            LOG_ERROR("Could not find debouncer timer for: %s", magic_enum::enum_name(timerId).data());
            return;
        }
        auto debounceTimerState = debounceTimers.at(timerId);
        debounceTimerState.gpio->DisableInterrupt(1U << static_cast<uint32_t>(debounceTimerState.pin));
        debounceTimerState.lastState = debounceTimerState.gpio->ReadPin(static_cast<uint32_t>(debounceTimerState.pin))
                                           ? KeyEvents::Released
                                           : KeyEvents::Pressed;
        if (debounceTimerState.timer != nullptr) {
            xTimerResetFromISR(debounceTimerState.timer, &xHigherPriorityTaskWoken);
        }
    }

    BaseType_t IRQHandler(uint32_t mask)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        auto timerId                        = DebounceTimerId::Invalid;

        gpio_sw->ClearPortInterrupts(1U << mask);

        if (mask & (1 << magic_enum::enum_integer(BoardDefinitions::BELL_SWITCHES_LEFT))) {
            timerId = DebounceTimerId::leftSideSwitch;
        }
        else if (mask & (1 << magic_enum::enum_integer(BoardDefinitions::BELL_SWITCHES_RIGHT))) {
            timerId = DebounceTimerId::rightSideSwitch;
        }
        else if (mask & (1 << magic_enum::enum_integer(BoardDefinitions::BELL_SWITCHES_CENTER))) {
            timerId = DebounceTimerId::lightCenterSwitch;
        }
        else if (mask & (1 << magic_enum::enum_integer(BoardDefinitions::BELL_SWITCHES_LATCH))) {
            timerId = DebounceTimerId::latchSwitch;
        }

        getDebounceTimer(xHigherPriorityTaskWoken, timerId);

        return xHigherPriorityTaskWoken;
    }

    BaseType_t wakeupIRQHandler()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        gpio_wakeup->ClearPortInterrupts(1U << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP));

        getDebounceTimer(xHigherPriorityTaskWoken, DebounceTimerId::wakeup);

        return xHigherPriorityTaskWoken;
    }

    void enableIRQ()
    {
        gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_CENTER));
        gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT));
        gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT));
        gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH));
    }

    void enableWakeupIRQ()
    {
        gpio_wakeup->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP));
    }

    void disableIRQ()
    {
        gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_CENTER));
        gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT));
        gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT));
        gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH));
    }

    void disableWakeupIRQ()
    {
        gpio_wakeup->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP));
    }

    std::vector<KeyEvent> getKeyEvents(NotificationSource notification)
    {
        std::vector<KeyEvent> out;
        KeyEvent keyEvent;

        switch (notification) {
        case NotificationSource::leftSideKeyPress:
            LOG_DEBUG("leftSideKeyPress");
            keyEvent = {KeyCodes::FnLeft, KeyEvents::Pressed};
            out.push_back(keyEvent);
            break;
        case NotificationSource::leftSideKeyRelease:
            LOG_DEBUG("leftSideKeyRelease");
            keyEvent = {KeyCodes::FnLeft, KeyEvents::Released};
            out.push_back(keyEvent);
            break;
        case NotificationSource::rightSideKeyPress:
            LOG_DEBUG("rightSideKeyPress");
            keyEvent = {KeyCodes::FnRight, KeyEvents::Pressed};
            out.push_back(keyEvent);
            break;
        case NotificationSource::rightSideKeyRelease:
            LOG_DEBUG("rightSideKeyRelease");
            keyEvent = {KeyCodes::FnRight, KeyEvents::Released};
            out.push_back(keyEvent);
            break;
        case NotificationSource::lightCenterKeyPress:
            LOG_DEBUG("lightCenterKeyPress");
            keyEvent = {KeyCodes::JoystickEnter, KeyEvents::Moved};
            out.push_back(keyEvent);
            break;
        case NotificationSource::lightCenterKeyRelease:
            LOG_DEBUG("lightCenterKeyRelease");
            keyEvent = {KeyCodes::JoystickEnter, KeyEvents::Moved};
            out.push_back(keyEvent);
            break;
        case NotificationSource::latchKeyPress:
            LOG_DEBUG("latchKeyPress");
            keyEvent = {KeyCodes::JoystickRight, KeyEvents::Moved};
            out.push_back(keyEvent);
            break;
        case NotificationSource::latchKeyRelease:
            LOG_DEBUG("latchKeyRelease");
            keyEvent = {KeyCodes::JoystickLeft, KeyEvents::Moved};
            out.push_back(keyEvent);
            break;
        case NotificationSource::wakeupEvent:
            /* Implement wakeup event */
            break;
        case NotificationSource::wakeupEventRelease:
            /* Implement wakeup event */
            break;
        }
        return out;
    }

} // namespace bsp::bell_switches
