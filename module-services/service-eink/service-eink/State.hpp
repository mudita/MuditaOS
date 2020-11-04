// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace service::eink
{
    enum class State
    {
        None,           //< not initialized
        Init,           //< service eink in startup
        PowerIdle,      //< service eink powered up in idle state - awaits data
        PowerPorcesing, //< service eink powered up, processing data
        PowerDown,
        PowerDownProcessing,
        PowerSleep,
        PowerSleepProcessing,
    };

}
