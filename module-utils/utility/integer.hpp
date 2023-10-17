// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace utils::integer
{

    /**
     * @brief number bits in bytes constant for greater clarity
     */
    constexpr inline auto BitsInByte = 8U;

    /**
     * @brief Allows to pass type as a return value with no overhead; used by
     * getIntegerType.
     *
     * @tparam T - type to return
     */
    template <typename T>
    struct TypeHolder
    {
        using type = T;
    };

    /**
     * @brief Discovers the type of integer which is best suited to hold
     * N bits of information, e.g.: returns std::uint8_t for 5 bits,
     * std::uint32_t for 18.
     *
     * @tparam Bits - number of bits which must an integer be able to hold.
     * @return integer type best suited to hold value Bits bits long.
     */
    template <unsigned int Bits>
    auto getIntegerType()
    {
        static_assert(Bits <= 64);

        if constexpr (Bits <= 8) {
            return TypeHolder<std::uint8_t>();
        }
        else if constexpr (Bits <= 16) {
            return TypeHolder<std::uint16_t>();
        }
        else if constexpr (Bits <= 32) {
            return TypeHolder<std::uint32_t>();
        }
        else {
            return TypeHolder<std::uint64_t>();
        }
    }

} // namespace utils::integer
