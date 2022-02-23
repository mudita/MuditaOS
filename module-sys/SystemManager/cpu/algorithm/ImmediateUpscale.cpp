// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImmediateUpscale.hpp"

namespace sys::cpu
{
    bsp::CpuFrequencyMHz ImmediateUpscale::calculateImplementation(const AlgorithmData&data)
    {
        auto now = data.sentinel.frequency;
        auto was = data.curentFrequency; 
        return std::max(now,was);
    }
} // namespace sys::cpu
