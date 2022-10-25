// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/filesystem_paths.hpp>

/**
 When both partitions are controlled by one fs driver (lwext4), we cannot mount
 one partition on /sys and second one on /sys/user, because for the driver
 it's indistinguishable whether we want to access second partition or
 create directory named "user" on the first one. This only worked before because
 we had two different drivers under the VFS - FAT and ext4.
 Thus the change of the paths structure.
**/

namespace
{
    constexpr inline auto PATH_SYS         = "/sys";
    constexpr inline auto PATH_USER        = "/user";
    constexpr inline auto PATH_CONF        = "/mfgconf";
    constexpr inline auto PATH_CURRENT     = "current";
    constexpr inline auto PATH_PREVIOUS    = "previous";
    constexpr inline auto PATH_UPDATES     = "updates";
    constexpr inline auto PATH_TMP         = "tmp";
    constexpr inline auto PATH_BACKUP      = "backup";
    constexpr inline auto PATH_FACTORY     = "factory";
    constexpr inline auto PATH_LOGS        = "logs";
    constexpr inline auto PATH_CRASH_DUMPS = "crash_dumps";
} // namespace

namespace purefs
{
    std::filesystem::path createPath(const std::string &parent, const std::string &child) noexcept
    {
        return std::filesystem::path{parent} / child;
    }

    namespace dir
    {
        std::filesystem::path getOSDiskPath() noexcept
        {
            return std::filesystem::path{PATH_SYS};
        }

        std::filesystem::path getUserDiskPath() noexcept
        {
            return std::filesystem::path{PATH_USER};
        }

        std::filesystem::path getMfgConfPath() noexcept
        {
            return std::filesystem::path{PATH_CONF};
        }

        std::filesystem::path getCurrentOSPath() noexcept
        {
            return getOSDiskPath() / PATH_CURRENT;
        }

        std::filesystem::path getPreviousOSPath() noexcept
        {
            return getOSDiskPath() / PATH_PREVIOUS;
        }

        std::filesystem::path getUpdatesOSPath() noexcept
        {
            return getUserDiskPath() / PATH_UPDATES; // TODO shouldn't it be on OS partition?
        }

        std::filesystem::path getTemporaryPath() noexcept
        {
            return getUserDiskPath() / PATH_TMP; // TODO shouldn't it be on OS partition?
        }

        std::filesystem::path getBackupOSPath() noexcept
        {
            return getUserDiskPath() / PATH_BACKUP; // TODO shouldn't it be on OS partition?
        }

        std::filesystem::path getFactoryOSPath() noexcept
        {
            return getOSDiskPath() / PATH_FACTORY;
        }

        std::filesystem::path getLogsPath() noexcept
        {
            return getUserDiskPath() / PATH_LOGS;
        }

        std::filesystem::path getCrashDumpsPath() noexcept
        {
            return getUserDiskPath() / PATH_CRASH_DUMPS;
        }
    } // namespace dir
} // namespace purefs
