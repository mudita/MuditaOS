// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "init_prof.hpp"
#include "FreeRTOS.h"
#include <cstdint>
#include <bsp/common.hpp>

extern "C"
{
#include "prof.h"
}

namespace prof
{
    void init()
    {
#if PROF_ON
        struct prof_pool_init_data init
        {
            32
        };
        prof_pool_init(init);
#endif
    }
} // namespace prof
