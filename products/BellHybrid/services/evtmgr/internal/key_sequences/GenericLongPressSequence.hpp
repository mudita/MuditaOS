// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractKeySequence.hpp"
#include <keymap/KeyMap.hpp>
#include <Timers/TimerHandle.hpp>

#include <chrono>
#include <memory>

template <KeyMap... keys> class GenericLongPressSequence : public AbstractKeySequence
{
    using Keys = std::array<KeyMap, sizeof...(keys)>;
    enum class State
    {
        Idle,
        InProgress,
        Ready,
        Abort
    };

  public:
    GenericLongPressSequence(sys::TimerHandle &&timer) : timer{std::move(timer)}
    {}

    void process(const RawKey &key) override
    {
        keyStates.set(mapKey(key), key.state == RawKey::State::Pressed);

        switch (state) {
        case State::Idle:
            if (keyStates.count() == keysToScanCount && keyStates.ifOnlySet(keysToScan)) {
                switch_to_in_progress();
            }
            break;
        case State::InProgress:
        case State::Ready:
            if (keyStates.count() > keysToScanCount) {
                switch_to_abort();
            }
            else if (keyStates.count() < keysToScanCount) {
                switch_to_idle();
            }
            break;
        case State::Abort:
            if (keyStates.count() == 0) {
                state = State::Idle;
            }
        }
    }

    void handleTimer()
    {
        state = State::Ready;
        ready();
    }

  private:
    void switch_to_abort()
    {
        state = State::Abort;
        abort();
        timer.stop();
    }

    void switch_to_idle()
    {
        state = State::Idle;
        idle();
    }

    void switch_to_in_progress()
    {
        state = State::InProgress;
        trigger();
        timer.stop();
        timer.start();
    }

    static constexpr Keys keysToScan      = {keys...};
    static constexpr auto keysToScanCount = keysToScan.size();
    State state                           = State::Idle;
    KeyStates keyStates;
    sys::TimerHandle timer;
};
