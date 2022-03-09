// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlgorithmFactory.hpp"
#include "SystemManager/cpu/algorithm/FrequencyHold.hpp"
#include <memory>

namespace sys::cpu
{

    AlgorithmFactory::PutResult AlgorithmFactory::emplace(sys::cpu::AlgoID id, std::unique_ptr<Algorithm> &&algorithm)
    {
        if (algorithms.find(id) != std::end(algorithms)) {
            algorithms[id] = std::move(algorithm);
            return PutResult::Replaced;
        }
        algorithms.emplace(id, std::move(algorithm));
        return PutResult::Added;
    }

    Algorithm *AlgorithmFactory::get(sys::cpu::AlgoID id)
    {
        if (auto el = algorithms.find(id); el != std::end(algorithms)) {
            return el->second.get();
        }
        return nullptr;
    }

    void AlgorithmFactory::remove(sys::cpu::AlgoID id)
    {
        algorithms.erase(id);
    }
} // namespace sys::cpu
