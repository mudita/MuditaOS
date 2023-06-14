// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/trng/trng.hpp"
#include <random>
#include <memory>

namespace bsp::trng
{
    std::uint32_t getRandomValue()
    {
        const auto trueRandomSource = std::make_unique<std::random_device>();
        return (*trueRandomSource)();
    }
} // namespace bsp::trng
