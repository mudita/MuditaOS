// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once
#include <cstdint>

namespace cellular::service
{

    class URCCounter
    {
      public:
        void count();
        void clearCounter();
        auto getCounter() -> uint32_t;

      private:
        uint32_t urcOccurences = 0;
    };
} // namespace cellular::service
