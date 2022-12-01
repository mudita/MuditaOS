// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/filesystem_paths.hpp>
#include <hal/boot_control.h>

namespace
{
    constexpr inline auto PATH_SYSTEM      = "/system";
    constexpr inline auto PATH_USER        = "/user";
    constexpr inline auto PATH_CONF        = "/mfgconf";
    constexpr inline auto PATH_DB          = "db";
    constexpr inline auto PATH_LOGS        = "log";
    constexpr inline auto PATH_CRASH_DUMPS = "crash_dumps";
    constexpr inline auto PATH_USER_MEDIA =
        "media"; // TODO this won't work with our current non-hierarchical MTP implementation
    constexpr inline auto PATH_TMP    = "temp";
    constexpr inline auto PATH_ASSETS = "assets";
    constexpr inline auto PATH_DATA   = "data";
} // namespace

namespace purefs
{
    std::filesystem::path createPath(const std::string &parent, const std::string &child) noexcept
    {
        return std::filesystem::path{parent} / child;
    }

    namespace dir
    {
        std::filesystem::path getSystemDiskPath() noexcept
        {
            return std::filesystem::path{PATH_SYSTEM};
        }

        std::filesystem::path getUserDiskPath() noexcept
        {
            return std::filesystem::path{PATH_USER};
        }

        std::filesystem::path getMfgConfPath() noexcept
        {
            return std::filesystem::path{PATH_CONF};
        }

        std::filesystem::path getDatabasesPath() noexcept
        {
            return getSystemDiskPath() / PATH_DB;
        }

        std::filesystem::path getLogsPath() noexcept
        {
            return getSystemDiskPath() / PATH_LOGS;
        }

        std::filesystem::path getCrashDumpsPath() noexcept
        {
            return getSystemDiskPath() / PATH_CRASH_DUMPS;
        }

        std::filesystem::path getUserMediaPath() noexcept
        {
            return getUserDiskPath() / PATH_USER_MEDIA;
        }

        std::filesystem::path getTemporaryPath() noexcept
        {
            return getUserDiskPath() / PATH_TMP;
        }

        std::filesystem::path getBootJSONPath() noexcept
        {
            return getUserDiskPath() / file::boot_json;
        }

        std::filesystem::path getAssetsDirPath() noexcept
        {
            return getSystemDiskPath() / PATH_ASSETS;
        }

        std::filesystem::path getSystemDataDirPath() noexcept
        {
            return getSystemDiskPath() / PATH_DATA;
        }

        std::filesystem::path getUserDataDirPath() noexcept
        {
            return getUserDiskPath() / PATH_DATA;
        }
    } // namespace dir
} // namespace purefs
