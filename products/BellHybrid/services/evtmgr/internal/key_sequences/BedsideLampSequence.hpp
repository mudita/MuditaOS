// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GenericLongPressSequence.hpp"
#include <Timers/TimerFactory.hpp>

class BedsideLampSequence : public GenericLongPressSequence<KeyMap::Frontlight>
{
  public:
    explicit BedsideLampSequence(sys::Service &service)
        : GenericLongPressSequence<KeyMap::Frontlight>{sys::TimerFactory::createSingleShotTimer(
              &service, "lampseq", std::chrono::milliseconds{2000}, [this](auto &) { handleTimer(); })}
    {}
};
