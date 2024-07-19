// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <board/rt1051/bsp/eink/ED028TC1.hpp>
#include <chip.hpp>

extern "C"
{
#include "dma_config.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
}
