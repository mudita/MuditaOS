// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "dateCommon.hpp"

namespace utils
{
    namespace time
    {
        struct FromTillDate
        {
            FromTillDate() = default;
            explicit FromTillDate(TimePoint from, TimePoint till) : from(std::move(from)), till(std::move(till))
            {}
            TimePoint from;
            TimePoint till;
        };
    } // namespace time
} // namespace utils
