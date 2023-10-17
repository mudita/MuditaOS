// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SystemManager/cpu/algorithm/AlgorithmChange.hpp"
#include "SystemManager/cpu/algorithm/AlgorithmID.hpp"
#include "SystemManager/SentinelView.hpp"

namespace sys::cpu
{
    struct UpdateResult
    {
        algorithm::Change changed         = algorithm::Change::NoChange;
        bsp::CpuFrequencyMHz frequencySet = bsp::CpuFrequencyMHz::Level_0;
        sentinel::View data{};
        AlgoID id = AlgoID::None;
    };
}; // namespace sys::cpu
