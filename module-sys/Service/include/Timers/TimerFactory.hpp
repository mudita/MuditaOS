// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include "TimerHandle.hpp"

namespace sys
{
    class Service; // Forward declaration
} // namespace sys

namespace sys
{
    class TimerFactory
    {
      public:
        static TimerHandle createSingleShotTimer(Service *parent,
                                                 const std::string &name,
                                                 std::chrono::milliseconds interval,
                                                 timer::TimerCallback &&callback);
        static TimerHandle createPeriodicTimer(Service *parent,
                                               const std::string &name,
                                               std::chrono::milliseconds interval,
                                               timer::TimerCallback &&callback);
    };
} // namespace sys
