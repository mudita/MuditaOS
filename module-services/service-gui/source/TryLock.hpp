// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <mutex.hpp>

class Trylock
{
    cpp_freertos::Mutex &mutex;
    bool locked = false;

  public:
    Trylock(cpp_freertos::Mutex &m, uint32_t timeout) : mutex(m)
    {
        locked = mutex.Lock(timeout);
    }

    ~Trylock()
    {
        if (locked) {
            mutex.Unlock();
        }
    }

    [[nodiscard]] auto isLocked() const noexcept
    {
        return locked;
    }
};
