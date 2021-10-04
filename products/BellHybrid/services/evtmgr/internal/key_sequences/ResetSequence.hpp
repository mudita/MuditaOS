// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GenericLongPressSequence.hpp"
#include <Timers/TimerFactory.hpp>

class ResetSequence : public GenericLongPressSequence<KeyMap::Back, KeyMap::Frontlight>
{
  public:
    explicit ResetSequence(sys::Service &service)
        : GenericLongPressSequence<KeyMap::Back, KeyMap::Frontlight>{sys::TimerFactory::createSingleShotTimer(
              &service, "rseq", std::chrono::milliseconds{10000}, [this](auto &) { handleTimer(); })}

    {}
};
