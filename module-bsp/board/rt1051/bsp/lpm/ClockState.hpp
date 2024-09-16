// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <fsl_clock.h>

namespace bsp
{
    [[nodiscard]] bool IsClockEnabled(clock_ip_name_t name);
}
