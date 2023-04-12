// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
extern "C"
{
#include "cmsis_gcc.h"
#include "fsl_drivers/fsl_src.h"
#include "macros.h"
}

void clearAndPrintBootReason();
