// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
#include <switches/LatchState.hpp>

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
    constexpr std::chrono::milliseconds centerKeyPressValidationTimeout{500ms};

    enum class KeyId : unsigned int
    {
        leftSideSwitch = 0,
        rightSideSwitch,
        lightCenterSwitch,
        latchSwitch,
        Invalid
    };

    void debounceTimerCallback(TimerHandle_t timerHandle);

    struct DebounceTimerState
    {
        const KeyId id{KeyId::Invalid};
        const std::shared_ptr<drivers::DriverGPIO> gpio;
        const BoardDefinitions pin{-1};
        volatile KeyEvents lastState{KeyEvents::Pressed};
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
        bool pressed    = false;
        bool changeFlag = false;

      public:
        static constexpr bool NO_STATE_CHANGE_INDICATION{false};
        void setPressed(bool changed = true)
        {
            cpp_freertos::LockGuard lock(latchFlagMutex);
            pressed    = true;
            changeFlag = changed;
        }

        void setReleased(bool changed = true)
        {
            cpp_freertos::LockGuard lock(latchFlagMutex);
            pressed    = false;
            changeFlag = changed;
        }

        bool isPressed()
        {
            cpp_freertos::LockGuard lock(latchFlagMutex);
            return pressed;
        }

        bool wasMarkedChanged()
        {
            cpp_freertos::LockGuard lock(latchFlagMutex);
            return changeFlag;
        }

        void setChanged()
        {
            changeFlag = true;
        }

        void clearChangedMark()
        {
            cpp_freertos::LockGuard lock(latchFlagMutex);
            changeFlag = false;
        }

    } latchEventFlag;

    static std::map<KeyId, DebounceTimerState> debounceTimers;

    static xQueueHandle qHandleIrq{};
    std::shared_ptr<DriverGPIO> gpio_sw;
    std::shared_ptr<DriverGPIO> gpio_center;

    NotificationSource getNotificationSource(KeyId keyId, KeyEvents state)
    {
        auto val = NotificationSource::Invalid;
        if (state == KeyEvents::Released) {
            switch (keyId) {
            case KeyId::leftSideSwitch:
                val = NotificationSource::leftSideKeyRelease;
                break;
            case KeyId::rightSideSwitch:
                val = NotificationSource::rightSideKeyRelease;
                break;
            case KeyId::lightCenterSwitch:
                val = NotificationSource::lightCenterKeyRelease;
                break;
            case KeyId::latchSwitch:
                val = NotificationSource::latchKeyRelease;
                break;
            default:
                break;
            }
        }
        else {
            switch (keyId) {
            case KeyId::leftSideSwitch:
                val = NotificationSource::leftSideKeyPress;
                break;
            case KeyId::rightSideSwitch:
                val = NotificationSource::rightSideKeyPress;
                break;
            case KeyId::lightCenterSwitch:
                val = NotificationSource::lightCenterKeyPress;
                break;
            case KeyId::latchSwitch:
                val = NotificationSource::latchKeyPress;
                break;
            default:
                break;
            }
        }
        return val;
    }

    void debounceTimerCallback(TimerHandle_t timerHandle)
    {
        auto timerState = static_cast<DebounceTimerState *>(pvTimerGetTimerID(timerHandle));
        xTimerStop(timerState->timer, 0);

        auto currentState = timerState->gpio->ReadPin(static_cast<uint32_t>(timerState->pin)) ? KeyEvents::Released
                                                                                              : KeyEvents::Pressed;
        if (currentState == timerState->lastState && qHandleIrq != nullptr) {
            if (timerState->id == KeyId::latchSwitch) {
                if (currentState == KeyEvents::Pressed) {
                    latchEventFlag.setPressed();
                }
                else {
                    latchEventFlag.setReleased();
                }
            }
            auto val = static_cast<std::uint16_t>(getNotificationSource(timerState->id, currentState));
            xQueueSend(qHandleIrq, &val, 0);
        }
        else {
            /** Even if we did not change the state of latch, the IRQ was called,
             * so lets skip next light center click
             */
            if (timerState->id == KeyId::latchSwitch) {
                latchEventFlag.setChanged();
            }
        }

        timerState->gpio->EnableInterrupt(1U << static_cast<uint32_t>(timerState->pin));
    }

    void debounceTimerCenterClickCallback(TimerHandle_t timerHandle)
    {
        auto timerState = static_cast<DebounceTimerState *>(pvTimerGetTimerID(timerHandle));
        xTimerStop(timerState->timer, 0);

        if (qHandleIrq != nullptr) {
            if (latchEventFlag.wasMarkedChanged()) {
                latchEventFlag.clearChangedMark();
            }
            else {
                auto val = static_cast<std::uint16_t>(getNotificationSource(timerState->id, KeyEvents::Released));
                xQueueSend(qHandleIrq, &val, 0);
            }
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

        if (timerState.id == KeyId::lightCenterSwitch) {
            state.createTimer(debounceTimerCenterClickCallback, centerKeyPressValidationTimeout);
        }
        else {
            state.createTimer(debounceTimerCallback, contactOscillationTimeout);
        }
    }

    void addLatchDebounceTimer(DebounceTimerState timerState)
    {
        debounceTimers.insert({timerState.id, timerState});
        DebounceTimerState &state = debounceTimers.find(timerState.id)->second;

        state.createTimer(debounceTimerCallback, contactOscillationTimeout);
        state.lastState = getLatchState();
        if (state.lastState == KeyEvents::Released) {
            latchEventFlag.setReleased(LatchEventFlag::NO_STATE_CHANGE_INDICATION);
        }
        else {
            latchEventFlag.setPressed(LatchEventFlag::NO_STATE_CHANGE_INDICATION);
        }
    }

    void configureSwitch(BoardDefinitions boardSwitch,
                         DriverGPIOPinParams::InterruptMode mode,
                         std::shared_ptr<drivers::DriverGPIO> gpio)
    {

        gpio->ClearPortInterrupts(1 << magic_enum::enum_integer(boardSwitch));
        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
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

        gpio_center = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_CENTER_SWITCH_GPIO),
                                         DriverGPIOParams{});

        configureSwitch(
            BoardDefinitions::BELL_SWITCHES_LATCH, DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge, gpio_sw);
        configureSwitch(
            BoardDefinitions::BELL_SWITCHES_LEFT, DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge, gpio_sw);
        configureSwitch(
            BoardDefinitions::BELL_SWITCHES_RIGHT, DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge, gpio_sw);
        configureSwitch(
            BoardDefinitions::BELL_CENTER_SWITCH, DriverGPIOPinParams::InterruptMode::IntFallingEdge, gpio_center);

        addDebounceTimer(DebounceTimerState{KeyId::leftSideSwitch, gpio_sw, BoardDefinitions::BELL_SWITCHES_LEFT});
        addDebounceTimer(DebounceTimerState{KeyId::rightSideSwitch, gpio_sw, BoardDefinitions::BELL_SWITCHES_RIGHT});
        addDebounceTimer(
            DebounceTimerState{KeyId::lightCenterSwitch, gpio_center, BoardDefinitions::BELL_CENTER_SWITCH});
        addLatchDebounceTimer(DebounceTimerState{
            KeyId::latchSwitch, gpio_sw, BoardDefinitions::BELL_SWITCHES_LATCH, KeyEvents::Released});

        if (getLatchState() == KeyEvents::Pressed) {
            latchEventFlag.setPressed(LatchEventFlag::NO_STATE_CHANGE_INDICATION);
            auto timerState =
                static_cast<DebounceTimerState *>(pvTimerGetTimerID(debounceTimers[KeyId::latchSwitch].timer));
            timerState->lastState = KeyEvents::Released;
        }
        clearStartupLatchInterrupt();
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
        gpio_center.reset();

        return kStatus_Success;
    }

    void getDebounceTimer(BaseType_t xHigherPriorityTaskWoken, KeyId timerId)
    {
        if (debounceTimers.find(timerId) == debounceTimers.end()) {
            LOG_ERROR("Could not find debouncer timer for: %s", magic_enum::enum_name(timerId).data());
            return;
        }
        DebounceTimerState &debounceTimerState = debounceTimers.at(timerId);
        debounceTimerState.gpio->DisableInterrupt(1U << static_cast<uint32_t>(debounceTimerState.pin));
        debounceTimerState.lastState = debounceTimerState.gpio->ReadPin(static_cast<uint32_t>(debounceTimerState.pin))
                                           ? KeyEvents::Released
                                           : KeyEvents::Pressed;
        if (debounceTimerState.timer != nullptr) {
            xTimerResetFromISR(debounceTimerState.timer, &xHigherPriorityTaskWoken);
        }
    }

    BaseType_t GPIO2SwitchesIRQHandler(uint32_t mask)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        auto timerId                        = KeyId::Invalid;

        if (mask & (1 << magic_enum::enum_integer(BoardDefinitions::BELL_SWITCHES_LEFT))) {
            timerId = KeyId::leftSideSwitch;
        }
        else if (mask & (1 << magic_enum::enum_integer(BoardDefinitions::BELL_SWITCHES_RIGHT))) {
            timerId = KeyId::rightSideSwitch;
        }
        else if (mask & (1 << magic_enum::enum_integer(BoardDefinitions::BELL_SWITCHES_LATCH))) {
            timerId = KeyId::latchSwitch;
        }

        getDebounceTimer(xHigherPriorityTaskWoken, timerId);

        return xHigherPriorityTaskWoken;
    }

    BaseType_t GPIO5SwitchesIRQHandler(uint32_t mask)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        auto keyID                          = KeyId::Invalid;
        if (mask & (1 << magic_enum::enum_integer(BoardDefinitions::BELL_CENTER_SWITCH))) {
            keyID = KeyId::lightCenterSwitch;
        }
        getDebounceTimer(xHigherPriorityTaskWoken, keyID);

        return xHigherPriorityTaskWoken;
    }

    void enableIRQ()
    {
        gpio_center->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_CENTER_SWITCH));
        gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT));
        gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT));
        gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH));
    }

    void disableIRQ()
    {
        gpio_center->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_CENTER_SWITCH));
        gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT));
        gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT));
        gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH));
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
        }
        return out;
    }

    bool isLatchPressed()
    {
        return latchEventFlag.isPressed();
    }

    void clearStartupLatchInterrupt()
    {
        // Knob sets up interrupt on startup because of hw design
        gpio_sw->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH));
    }

} // namespace bsp::bell_switches
