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

    AlgorithmResult AlgorithmFactory::calculate(const std::list<sys::cpu::AlgoID> &algorithms,
                                                cpu::AlgorithmData &data,
                                                AlgoID *used)
    {
        for (auto id : algorithms) {
            if (auto algo = get(id); algo != nullptr) {
                if (auto result = algo->calculate(data); result.change != sys::cpu::algorithm::Change::NoChange) {
                    if (used != nullptr) {
                        *used = id;
                    }
                    return result;
                }
            }
        }
        return sys::cpu::AlgorithmResult{sys::cpu::algorithm::Change::NoChange, bsp::CpuFrequencyMHz::Level_6};
    }

    void AlgorithmFactory::reset(const std::list<sys::cpu::AlgoID> &algorithms)
    {
        for (auto again : algorithms) {
            if (auto al = get(again); al != nullptr) {
                al->reset();
            }
        }
    }

} // namespace sys::cpu
