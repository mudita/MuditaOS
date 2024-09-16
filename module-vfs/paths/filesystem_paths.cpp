// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <purefs/filesystem_paths.hpp>

namespace
{
    constexpr auto PATH_SYSTEM      = "/system";
    constexpr auto PATH_USER        = "/user";
    constexpr auto PATH_CONF        = "/mfgconf";
    constexpr auto PATH_DB          = "db";
    constexpr auto PATH_LOGS        = "log";
    constexpr auto PATH_CRASH_DUMPS = "crash_dumps";
    constexpr auto PATH_USER_MEDIA  = "media";
    constexpr auto PATH_TMP         = "temp";
    constexpr auto PATH_ASSETS      = "assets";
    constexpr auto PATH_DATA        = "data";
    constexpr auto PATH_VAR         = "var";
} // namespace

namespace purefs
{
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

        std::filesystem::path getSystemVarDirPath() noexcept
        {
            return getSystemDiskPath() / PATH_VAR;
        }
    } // namespace dir
} // namespace purefs
