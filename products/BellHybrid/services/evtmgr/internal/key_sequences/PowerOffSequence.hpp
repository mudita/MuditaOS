// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "LongPressSequence.hpp"

class PowerOffSequence : public LongPressSequence
{
  public:
    explicit PowerOffSequence(sys::Service &service,
                              std::chrono::milliseconds timeout = std::chrono::milliseconds{5000})
        : LongPressSequence(KeyMap::Back, service, timeout)
    {}
};
