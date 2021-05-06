// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-bsp/board/rt1051/common/fsl_drivers/fsl_common.h"

static inline bool PWM_GetPwmGeneratorState(PWM_Type *base, uint8_t subModulesToStop)
{
    return (base->MCTRL & PWM_MCTRL_RUN(subModulesToStop)) != 0;
}
