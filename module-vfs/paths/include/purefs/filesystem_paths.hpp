// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <filesystem>

namespace purefs
{
    namespace dir
    {
        std::filesystem::path getSystemDiskPath() noexcept;
        std::filesystem::path getUserDiskPath() noexcept;
        std::filesystem::path getMfgConfPath() noexcept;
        std::filesystem::path getDatabasesPath() noexcept;
        std::filesystem::path getLogsPath() noexcept;
        std::filesystem::path getCrashDumpsPath() noexcept;
        std::filesystem::path getUserMediaPath() noexcept;
        std::filesystem::path getTemporaryPath() noexcept;
        std::filesystem::path getBootJSONPath() noexcept;
        std::filesystem::path getAssetsDirPath() noexcept;
        std::filesystem::path getSystemDataDirPath() noexcept;
        std::filesystem::path getSystemVarDirPath() noexcept;

    } // namespace dir

    namespace file
    {
        inline constexpr auto boot_json    = "boot.json";
        inline constexpr auto os_bin       = "os.bin";
        inline constexpr auto version_json = "version.json";
    } // namespace file

    namespace buffer
    {
        inline constexpr auto tar_buf = 1024;
    }
} // namespace purefs
