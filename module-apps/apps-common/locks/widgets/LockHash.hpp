// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/data/LockData.hpp>

#include <cstdint>
#include <vector>
#include <functional>

template <> struct std::hash<std::vector<unsigned int>>
{
    const static unsigned int digit_multiplier = 10;
    uint32_t operator()(std::vector<unsigned int> const &input) const noexcept
    {
        uint32_t value = 0;
        for (auto i : input) {
            value = digit_multiplier * value + i;
        }
        return value;
    }
};

static inline uint32_t getHash(locks::LockInput input)
{
    static std::hash<std::vector<unsigned int>> hashEngine;
    return hashEngine(input);
}
