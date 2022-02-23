// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Algorithm.hpp"

namespace sys::cpu
{
    bsp::CpuFrequencyMHz Algorithm::calculate(const AlgorithmData &data)
    {
        return calculateImplementation(data);
    }

    void Algorithm::reset()
    {
        resetImplementation();
    }
} // namespace sys::cpu
