// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
extern "C"
{
#include "cmsis/cmsis_gcc.h"
#include "cmsis/core_cm7.h"
#include "fsl_drivers/fsl_src.h"
#include <board/macros.h>
}

void clearAndPrintBootReason();
