// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/filesystem_paths.hpp>

namespace
{
    constexpr auto PATH_SYS          = "/sys";
    constexpr auto PATH_CONF         = "/mfgconf";
    constexpr auto PATH_USER         = "user";
    constexpr auto PATH_CURRENT      = "current";
    constexpr auto PATH_PREVIOUS     = "previous";
    constexpr auto PATH_UPDATES      = "updates";
    constexpr auto PATH_TMP          = "tmp";
    constexpr auto PATH_BACKUP       = "backup";
    constexpr inline auto PATH_SYNC  = "sync";
    constexpr auto PATH_FACTORY      = "factory";
    constexpr auto PATH_LOGS         = "logs";
    constexpr auto PATH_CRASH_DUMPS  = "crash_dumps";
    constexpr auto PATH_USER_AUDIO   = "audio";
    constexpr auto PATH_USER_STORAGE = "storage";
    constexpr auto eMMC_disk         = PATH_SYS;
} // namespace

namespace purefs
{
    std::filesystem::path createPath(const std::string &parent, const std::string &child) noexcept
    {
        return std::filesystem::path{parent} / child;
    }

    namespace dir
    {
        std::filesystem::path getRootDiskPath() noexcept
        {
            return std::filesystem::path{eMMC_disk};
        }

        std::filesystem::path getMfgConfPath() noexcept
        {
            return std::filesystem::path{PATH_CONF};
        }

        std::filesystem::path getUserDiskPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_USER;
        }

        std::filesystem::path getCurrentOSPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_CURRENT;
        }

        std::filesystem::path getPreviousOSPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_PREVIOUS;
        }

        std::filesystem::path getUpdatesOSPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_USER / PATH_UPDATES;
        }

        std::filesystem::path getTemporaryPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_USER / PATH_TMP;
        }

        std::filesystem::path getBackupOSPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_USER / PATH_BACKUP;
        }

        std::filesystem::path getSyncPackagePath() noexcept
        {
            return getUserDiskPath() / PATH_SYNC;
        }

        std::filesystem::path getFactoryOSPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_FACTORY;
        }

        std::filesystem::path getLogsPath() noexcept
        {
            return getUserDiskPath() / PATH_LOGS;
        }

        std::filesystem::path getCrashDumpsPath() noexcept
        {
            return getUserDiskPath() / PATH_CRASH_DUMPS;
        }
        std::filesystem::path getUserAudioPath() noexcept
        {
            return getUserDiskPath() / PATH_USER_AUDIO;
        }
        std::filesystem::path getUserStoragePath() noexcept
        {
            return getUserDiskPath() / PATH_USER_STORAGE;
        }
    } // namespace dir
} // namespace purefs
