// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractKeySequence.hpp"
#include <keymap/KeyMap.hpp>
#include <Timers/TimerFactory.hpp>

class LongPressSequence : public AbstractKeySequence
{
  public:
    enum class State
    {
        Idle,
        InProgress,
        Ready
    };
    LongPressSequence(KeyMap keyToScan, sys::Service &service, std::chrono::milliseconds timeout);

    void process(const RawKey &key) override;

  private:
    bool gate{false};
    KeyStates keyStates;
    KeyMap keyToScan;
    State state = State::Idle;
    std::chrono::milliseconds timeout;
    sys::TimerHandle timerHandle;
};
