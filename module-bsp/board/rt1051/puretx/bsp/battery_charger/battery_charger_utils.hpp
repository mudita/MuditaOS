// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <bitset>

namespace bsp::battery_charger::utils
{
    template <typename T> int twosComplimentToInt(T toConvert)
    {
        constexpr auto bitSize = sizeof(T) * 8;
        std::bitset<bitSize> bitset{toConvert};

        if (bitset[bitSize - 1]) {
            bitset = std::bitset<bitSize>(toConvert - 1);
            bitset.flip();
            return static_cast<int>(bitset.to_ulong() * -1);
        }
        else {
            return static_cast<int>(toConvert);
        }
    }
} // namespace bsp::battery_charger::utils
