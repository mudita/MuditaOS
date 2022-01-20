// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include "FreeRTOS.h"

namespace hal::battery
{
    BaseType_t INTBHandlerIRQ();
    BaseType_t INOKBHandlerIRQ();
} // namespace hal::battery
