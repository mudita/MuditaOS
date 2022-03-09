// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SystemManager/cpu/algorithm/AlgorithmID.hpp"
#include "SystemManager/SentinelView.hpp"

namespace sys::cpu
{
    struct UpdateResult
    {
        enum class Result
        {
            UpScaled,   /// frequency risen
            Downscaled, /// frequency downscaled
            NoChange    /// nothing to do
        };
        Result changed                    = Result::NoChange;
        bsp::CpuFrequencyMHz frequencySet = bsp::CpuFrequencyMHz::Level_0;
        sentinel::View data{};
        AlgoID id = AlgoID::None;
    };
}; // namespace sys::cpu
