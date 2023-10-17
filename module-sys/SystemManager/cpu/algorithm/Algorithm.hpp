// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlgorithmID.hpp"
#include "AlgorithmChange.hpp"
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

    struct AlgorithmResult
    {
        /// is change required by algorithm
        algorithm::Change change;
        // value requested by algorithm
        bsp::CpuFrequencyMHz value;
    };

    class Algorithm
    {
      private:
        [[nodiscard]] virtual AlgorithmResult calculateImplementation(const AlgorithmData &) = 0;
        virtual void resetImplementation()
        {}

      public:
        [[nodiscard]] virtual AlgorithmResult calculate(const AlgorithmData &) final;
        virtual void reset() final;

        virtual ~Algorithm() = default;
    };
} // namespace sys::cpu
