// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "common.hpp"
#include "FreeRTOS.h"
#include "portmacro.h"
#include <string>

namespace sys
{

    namespace sentinel
    {
        struct View
        {
            UBaseType_t ownerTCBNumber = 0;
            /// name of sentinel thread responsible for curent minimum load
            std::string name;
            /// curent minimum frequency set in sentinel
            bsp::CpuFrequencyMHz minFrequency = bsp::CpuFrequencyMHz::Level_0;
            /// textual information on what actually happens
            std::string reason;
        };
    }; // namespace sentinel
} // namespace sys
