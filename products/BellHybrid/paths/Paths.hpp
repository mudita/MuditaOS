// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <filesystem>

namespace paths
{
    namespace audio
    {
        std::filesystem::path user() noexcept;
        std::filesystem::path proprietary() noexcept;
        std::filesystem::path alarm() noexcept;
        std::filesystem::path preWakeup() noexcept;
        std::filesystem::path snooze() noexcept;
        std::filesystem::path bedtimeReminder() noexcept;
        std::filesystem::path relaxation() noexcept;
        std::filesystem::path meditation() noexcept;
    } // namespace audio

} // namespace paths
