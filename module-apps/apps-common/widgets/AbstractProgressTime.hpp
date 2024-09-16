// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <time/time_constants.hpp>
#include <cstdint>

namespace gui
{
    class AbstractProgressTime
    {
      public:
        virtual ~AbstractProgressTime()                = default;
        virtual void updateTime(std::uint32_t currentSeconds) = 0;
        virtual std::chrono::seconds getRefreshTime()  = 0;
    };
} // namespace gui
