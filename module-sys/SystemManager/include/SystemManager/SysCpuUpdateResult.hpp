// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SystemManager/CpuGovernor.hpp"
namespace sys::cpu
{
    struct UpdateResult
    {
        bool changed                      = false;
        bsp::CpuFrequencyMHz frequencySet = bsp::CpuFrequencyMHz::Level_0;
        sentinel::Data data{};
    };
}; // namespace sys::cpu
