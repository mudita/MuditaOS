// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <limits>
#include <fsl_trng.h>

namespace bsp
{
    struct RT1051Trng
    {
      public:
        using result_type = std::uint32_t;

        RT1051Trng()
        {}

        RT1051Trng(const RT1051Trng &) = delete;
        void operator=(const RT1051Trng &) = delete;

        static constexpr result_type min()
        {
            return std::numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return std::numeric_limits<result_type>::max();
        }

        result_type operator()()
        {
            return generateRandomNumber();
        }

      private:
        result_type generateRandomNumber();
    };
} // namespace bsp
