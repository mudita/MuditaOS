// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <board/rt1051/bsp/eink/ED028TC1.h>
#include <chip.hpp>

extern "C"
{
#include "dma_config.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
}
