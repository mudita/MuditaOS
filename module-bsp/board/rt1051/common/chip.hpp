// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
extern "C"
{
#include "cmsis_gcc.h"
#include "core_cm7.h"
#include "fsl_drivers/fsl_src.h"
#include "macros.h"
}

void clearAndPrintBootReason();
