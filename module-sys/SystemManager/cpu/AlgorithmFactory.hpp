// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "algorithm/Algorithm.hpp"
#include <list>
#include <map>
#include <memory>

namespace sys::cpu
{

    class AlgorithmFactory
    {
        std::map<sys::cpu::AlgoID, std::unique_ptr<Algorithm>> algorithms;

      public:
        Algorithm *get(sys::cpu::AlgoID);
        enum class PutResult
        {
            Added,
            Replaced,
        };
        PutResult emplace(sys::cpu::AlgoID id, std::unique_ptr<Algorithm> &&algorithm);

        /// use algorithms from factory depending on list to calculate frequency
        /// return used algorithm in used paramter
        AlgorithmResult calculate(const std::list<sys::cpu::AlgoID> &algorithms,
                                  cpu::AlgorithmData &data,
                                  AlgoID *used = nullptr);
        /// reset internal algorithms data
        void reset(const std::list<sys::cpu::AlgoID> &algorithms);

        void remove(sys::cpu::AlgoID id);
    };
}; // namespace sys::cpu
