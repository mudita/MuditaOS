// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrequencyHold.hpp"
#include <algorithm>

namespace sys::cpu
{
    FrequencyHold::FrequencyHold(bsp::CpuFrequencyMHz toHold, const bsp::PowerProfile &profile)
        : toHold(toHold), profile(profile)
    {}

    bsp::CpuFrequencyMHz FrequencyHold::calculateImplementation(const AlgorithmData &data)
    {
        return std::max(toHold, profile.minimalFrequency);
    }
}; // namespace sys::cpu
