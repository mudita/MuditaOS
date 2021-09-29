// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <filesystem>

namespace purefs
{
    std::filesystem::path createPath(const std::string &parent, const std::string &child) noexcept;

    namespace dir
    {
        std::filesystem::path getRootDiskPath() noexcept;
        std::filesystem::path getMfgConfPath() noexcept;
        std::filesystem::path getUserDiskPath() noexcept;
        std::filesystem::path getCurrentOSPath() noexcept;
        std::filesystem::path getPreviousOSPath() noexcept;
        std::filesystem::path getUpdatesOSPath() noexcept;
        std::filesystem::path getTemporaryPath() noexcept;
        std::filesystem::path getBackupOSPath() noexcept;
        std::filesystem::path getFactoryOSPath() noexcept;
        std::filesystem::path getLogsPath() noexcept;
        std::filesystem::path getCrashDumpsPath() noexcept;
    } // namespace dir

    namespace file
    {
        constexpr inline auto boot_json = ".boot.json";
        constexpr inline auto boot_bin  = "boot.bin";
        constexpr inline auto version_json = "version.json";
    } // namespace file

    namespace extension
    {
        constexpr inline auto tar = ".tar";
    }

    namespace buffer
    {
        constexpr inline auto tar_buf = 8192 * 4;
    }
} // namespace purefs
