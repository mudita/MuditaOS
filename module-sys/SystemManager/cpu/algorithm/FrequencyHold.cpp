// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrequencyHold.hpp"
#include <algorithm>
#include <list>

namespace sys::cpu
{
    FrequencyHold::FrequencyHold(bsp::CpuFrequencyMHz toHold, const bsp::PowerProfile &profile)
        : toHold(toHold), profile(profile)
    {}

    AlgorithmResult FrequencyHold::calculateImplementation(const AlgorithmData &data)
    {
        const auto retval = std::max(toHold, profile.minimalFrequency);
        if (toHold > data.curentFrequency) {
            return {algorithm::Change::UpScaled, retval};
        }
        else if (toHold < data.curentFrequency) {
            return {algorithm::Change::Downscaled, retval};
        }
        return {algorithm::Change::Hold, retval};
    }
}; // namespace sys::cpu
