// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <vector>
#include <functional>

template <> struct std::hash<std::vector<unsigned int>>
{
    const static unsigned int digit_multiplier = 10;
    uint32_t operator()(std::vector<unsigned int> const &pin) const noexcept
    {
        uint32_t value = 0;
        for (auto i : pin) {
            value = digit_multiplier * value + i;
        }
        return value;
    }
};

static inline uint32_t GetPinHash(const std::vector<unsigned int> &pin)
{
    static std::hash<std::vector<unsigned int>> hashEngine;
    return hashEngine(pin);
}
