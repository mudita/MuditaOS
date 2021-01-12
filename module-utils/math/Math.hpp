// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <cmath>

namespace trigonometry
{
    using SideLength = std::int32_t;
    using Degrees    = std::int32_t;
    using Radians    = double;

    constexpr Degrees FullAngle  = 360;
    constexpr Degrees HalfAngle  = 180;
    constexpr Degrees RightAngle = 90;

    constexpr static inline auto toRadians(Degrees degrees) noexcept -> Radians
    {
        return degrees * M_PI / HalfAngle;
    }

    struct AdjacentSide
    {
        static auto fromAngle(Radians angle, SideLength hypotenuse) -> SideLength
        {
            return std::lround(std::cos(angle) * hypotenuse);
        }

        static auto fromCosine(double cosine, SideLength hypotenuse) -> SideLength
        {
            return std::lround(cosine * hypotenuse);
        }
    };

    struct OppositeSide
    {
        static auto fromAngle(Radians angle, SideLength hypotenuse) -> SideLength
        {
            return std::lround(std::sin(angle) * hypotenuse);
        }

        static auto fromSine(double sine, SideLength hypotenuse) -> SideLength
        {
            return std::lround(sine * hypotenuse);
        }
    };
} // namespace trigonometry

namespace binary
{
    constexpr static inline auto isPowerOfTwo(unsigned int x) -> bool
    {
        return x == 0 ? false : __builtin_popcount(x) == 1;
    }

    static inline auto floorPowerOfTwo(unsigned int x) -> unsigned int
    {
        constexpr auto xBitCount = sizeof(x) * 8;

        if (x == 0) {
            return 0;
        }
        else if (isPowerOfTwo(x)) {
            return x;
        }

        return 1 << (xBitCount - __builtin_clz(x) - 1);
    }

    static inline auto ceilPowerOfTwo(unsigned int x) -> unsigned int
    {
        constexpr auto xBitCount = sizeof(x) * 8;
        auto leadingZeroes       = __builtin_clz(x);

        if (leadingZeroes == 0 || x == 0) {
            return 0;
        }
        else if (isPowerOfTwo(x)) {
            return x;
        }

        return 1 << (xBitCount - leadingZeroes);
    }

} // namespace binary
