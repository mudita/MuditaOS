// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <filesystem>

namespace purefs
{
    std::filesystem::path createPath(const std::string &parent, const std::string &child) noexcept;

    namespace dir
    {
        std::filesystem::path getSystemDiskPath() noexcept;
        std::filesystem::path getUserDiskPath() noexcept;
        std::filesystem::path getMfgConfPath() noexcept;
        std::filesystem::path getDatabasesPath() noexcept;
        std::filesystem::path getUpdateDirPath() noexcept;
        std::filesystem::path getSyncPackagePath() noexcept;
        std::filesystem::path getLogsPath() noexcept;
        std::filesystem::path getCrashDumpsPath() noexcept;
        std::filesystem::path getUserMediaPath() noexcept;
        std::filesystem::path getTemporaryPath() noexcept;
        std::filesystem::path getBackupOSPath() noexcept;
        std::filesystem::path getBootJSONPath() noexcept;
        std::filesystem::path getAssetsDirPath() noexcept;
        std::filesystem::path getSystemDataDirPath() noexcept;
        std::filesystem::path getUserDataDirPath() noexcept;
    } // namespace dir

    namespace file
    {
        constexpr inline auto boot_json    = "boot.json";
        constexpr inline auto boot_bin     = "boot.bin";
        constexpr inline auto version_json = "version.json";
    } // namespace file

    namespace buffer
    {
        constexpr inline auto tar_buf = 1024;
    }
} // namespace purefs
