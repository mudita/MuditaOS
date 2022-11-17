// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/filesystem_paths.hpp>

namespace
{
    constexpr inline auto PATH_SYSTEM       = "/system";
    constexpr inline auto PATH_USER         = "/user";
    constexpr inline auto PATH_CONF         = "/mfgconf";
    constexpr inline auto PATH_DB           = "db";
    constexpr inline auto PATH_FACTORY      = "factory";
    constexpr inline auto PATH_UPDATE       = "update";
    constexpr inline auto PATH_SYNC         = "sync";
    constexpr inline auto PATH_LOGS         = "logs";
    constexpr inline auto PATH_CRASH_DUMPS  = "crash_dumps";
    constexpr inline auto PATH_USER_MEDIA =
        "media"; // TODO this won't work with our current non-hierarchical MTP implementation
    constexpr inline auto PATH_TMP    = "tmp";
    constexpr inline auto PATH_BACKUP = "backup";
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

        std::filesystem::path getUpdateDirPath() noexcept
        {
            return getUserDiskPath() / PATH_UPDATE;
        }

        std::filesystem::path getSyncPackagePath() noexcept
        {
            return getUserDiskPath() / PATH_SYNC; // TODO what's this?
        }

        std::filesystem::path getFactoryDBsPath() noexcept
        {
            return getDatabasesPath() / PATH_FACTORY; // TODO this might not be required in OS
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
            return getSystemDiskPath() / PATH_TMP; // TODO is it needed + if it is, where it should be placed
        }

        std::filesystem::path getBackupOSPath() noexcept
        {
            return getUserDiskPath() / PATH_BACKUP; // TODO is it still needed?
        }
    } // namespace dir
} // namespace purefs
