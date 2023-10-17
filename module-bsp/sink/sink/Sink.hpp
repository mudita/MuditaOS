// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstddef>

class Sink
{
  public:
    virtual ~Sink()                                 = default;
    virtual void put(const char *data, size_t size) = 0;
};
