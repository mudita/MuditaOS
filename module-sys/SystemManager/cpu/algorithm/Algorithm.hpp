// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlgorithmID.hpp"
#include "SystemManager/SentinelView.hpp"
#include "common.hpp"

namespace sys::cpu
{
    struct AlgorithmData
    {
        unsigned int CPUload                 = 0;
        bsp::CpuFrequencyMHz curentFrequency = bsp::CpuFrequencyMHz::Level_6;
        sentinel::View sentinel;
    };

    class Algorithm
    {
      private:
        [[nodiscard]] virtual bsp::CpuFrequencyMHz calculateImplementation(const AlgorithmData &) = 0;
        virtual void resetImplementation()
        {}

      public:
        [[nodiscard]] virtual bsp::CpuFrequencyMHz calculate(const AlgorithmData &) final;
        virtual void reset() final;

        virtual ~Algorithm() = default;
    };
} // namespace sys::cpu
