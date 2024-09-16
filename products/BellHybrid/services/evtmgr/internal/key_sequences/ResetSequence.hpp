// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "GenericLongPressSequence.hpp"
#include <Timers/TimerFactory.hpp>

class ResetSequence : public GenericLongPressSequence<KeyMap::Back, KeyMap::Frontlight>
{
  public:
    explicit ResetSequence(sys::Service &service)
        : GenericLongPressSequence<KeyMap::Back, KeyMap::Frontlight>{sys::TimerFactory::createSingleShotTimer(
              &service, "rseq", std::chrono::milliseconds{5500}, [this](auto &) { handleTimer(); })}

    {}
};
