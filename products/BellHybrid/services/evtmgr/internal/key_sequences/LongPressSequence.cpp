// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LongPressSequence.hpp"

LongPressSequence::LongPressSequence(KeyMap keyToScan, sys::Service &service, std::chrono::milliseconds timeout)
    : keyToScan{keyToScan}
{
    timerHandle = sys::TimerFactory::createSingleShotTimer(&service, "lpressseq", timeout, [this](auto &) {
        state = State::Ready;
        ready();
    });
}
void LongPressSequence::process(const RawKey &key)
{
    keyStates.set(mapKey(static_cast<gui::KeyCode>(key.keyCode)), key.state == RawKey::State::Pressed);

    if (gate = keyStates.state(keyToScan) && not keyStates.ifOnlySet(keyToScan); gate) {
        if (state != State::Idle) {
            timerHandle.stop();
            state = State::Idle;
            abort();
        }
        return;
    }

    if (keyStates.state(keyToScan) && state == State::Idle) {
        timerHandle.stop();
        timerHandle.start();
        state = State::InProgress;
        trigger();
    }

    if (not keyStates.state(keyToScan) && state != State::Idle) {
        timerHandle.stop();
        state = State::Idle;
        idle();
    }
}
