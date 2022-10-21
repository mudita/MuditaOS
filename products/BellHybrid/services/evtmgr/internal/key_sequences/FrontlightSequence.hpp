// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GenericLongPressSequence.hpp"
#include <Timers/TimerFactory.hpp>

class FrontlightSequence : public GenericLongPressSequence<KeyMap::Frontlight>
{
  public:
    explicit FrontlightSequence(sys::Service &service)
        : GenericLongPressSequence<KeyMap::Frontlight>{sys::TimerFactory::createSingleShotTimer(
              &service, "frontseq", std::chrono::milliseconds{30}, [this](auto &) { handleTimer(); })}
    {}
};
