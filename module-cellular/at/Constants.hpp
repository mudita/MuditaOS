// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>

namespace at
{
    constexpr std::chrono::milliseconds default_timeout{
        5000}; /// applies for all timeout values smaller than itself - docs timeout < default_timeout
    constexpr std::chrono::milliseconds default_long_timeout{
        18000}; /// applies for all timeout values between default_timeout and istelf - default_timeout <= docs timout <
                /// default_long_timeout
} // namespace at
