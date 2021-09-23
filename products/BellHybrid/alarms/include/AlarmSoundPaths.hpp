// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <filesystem>

namespace alarms::paths
{
    std::filesystem::path getMusicDir() noexcept;
    std::filesystem::path getPreWakeUpChimesDir() noexcept;
    std::filesystem::path getSnoozeChimesDir() noexcept;
} // namespace alarms::paths
