// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Algorithm.hpp"
#include "lpm/PowerProfile.hpp"
#include <functional>
#include <utility>

namespace sys::cpu
{
    class ImmediateUpscale : public Algorithm
    {
      public:
        AlgorithmResult calculateImplementation(const AlgorithmData &data) override;
    };
} // namespace sys::cpu
