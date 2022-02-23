// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "algorithm/Algorithm.hpp"
#include <map>
#include <memory>


namespace sys::cpu {

    class AlgorithmFactory
    {
        std::map<sys::cpu::AlgoID, std::unique_ptr<Algorithm>> algorithms;

      public:
        Algorithm* get(sys::cpu::AlgoID);
        enum class PutResult {
            Added,
            Replaced,
        };
        PutResult emplace(sys::cpu::AlgoID id, std::unique_ptr<Algorithm>&&algorithm);
        void remove(sys::cpu::AlgoID id);
    };
};
