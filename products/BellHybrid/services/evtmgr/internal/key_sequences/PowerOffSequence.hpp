// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GenericLongPressSequence.hpp"
#include <Timers/TimerFactory.hpp>

class PowerOffSequence : public GenericLongPressSequence<KeyMap::Back>
{
  public:
    explicit PowerOffSequence(sys::Service &service)
        : GenericLongPressSequence<KeyMap::Back>{sys::TimerFactory::createSingleShotTimer(
              &service, "poffseq", std::chrono::milliseconds{10000}, [this](auto &) { handleTimer(); })}
    {}
};
