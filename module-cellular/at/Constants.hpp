// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>

namespace at
{
    constexpr std::chrono::milliseconds default_timeout{300};            /// if you've checked that it's ok
    constexpr std::chrono::milliseconds default_doc_timeout{5000};       /// if unsure - take this
    constexpr std::chrono::milliseconds default_long_doc_timeout{15000}; /// long timeout from docs
} // namespace at
