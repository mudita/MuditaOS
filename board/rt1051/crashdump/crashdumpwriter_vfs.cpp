// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "crashdumpwriter_vfs.hpp"
#include <crashdump-serial-number/crashdump_serial_number.hpp>
#include <cstdio>
#include <fcntl.h>
#include "purefs/vfs_subsystem.hpp"
#include <purefs/filesystem_paths.hpp>
#include <exit_backtrace.h>

#include <log/log.hpp>
#include <filesystem>
#include <stdint.h>
#include <unistd.h>
#include <chrono>
#include <set>

namespace
{
    constexpr inline auto suffix = "_crashdump.hex";

    // Crashdump filename pattern:
    // [serial-number]_[timestamp-in-seconds]_crashdump.hex

    inline std::string generate_crashdump_filename()
    {
        const auto crash_time =
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        auto filename = std::string("/") + crashdump::getSerialNumber() + "_" + std::to_string(crash_time) + suffix;
        return filename;
    }
} // namespace

namespace crashdump
{

    void CrashDumpWriterVFS::openDump()
    {
        const auto crashDumpFilePath = purefs::dir::getCrashDumpsPath().string() + generate_crashdump_filename();

        LOG_INFO("Crash dump %s preparing ...", crashDumpFilePath.c_str());
        file = std::fopen(crashDumpFilePath.c_str(), "w");
        if (!file) {
            LOG_FATAL("Failed to open crash dump file errno %i", errno);
            _exit_backtrace(-1, false);
        }
    }

    void CrashDumpWriterVFS::saveDump()
    {
        LOG_INFO("Crash dump create finished.");
        fflush(file);
        fsync(fileno(file));
        std::fclose(file);
    }

    void CrashDumpWriterVFS::deleteOldDump()
    {
        std::set<std::filesystem::path> crashdumps{};
        for (const auto &entry : std::filesystem::directory_iterator(purefs::dir::getCrashDumpsPath())) {
            std::cout << entry.path() << std::endl;
            crashdumps.insert(entry.path());
        }

        if (crashdumps.size() > maxFilesCount) {
            auto crashdump_to_delete = crashdumps.begin();
            LOG_INFO("Deleting %s ...", crashdump_to_delete->c_str());
            if (not std::filesystem::remove(crashdump_to_delete->c_str())) {
                LOG_WARN("File: %s was not deleted.", crashdump_to_delete->c_str());
            }
        }
    }

    void CrashDumpWriterVFS::writeBytes(const uint8_t *buff, std::size_t size)
    {
        if (std::fwrite(buff, sizeof(*buff), size, file) != size) {
            LOG_FATAL("Unable to write crash dump errno: %i", errno);
            _exit_backtrace(-1, false);
        }
    }

    void CrashDumpWriterVFS::writeHalfWords(const uint16_t *buff, std::size_t size)
    {
        if (std::fwrite(buff, sizeof(*buff), size, file) != size) {
            LOG_FATAL("Unable to write crash dump errno: %i", errno);
            _exit_backtrace(-1, false);
        }
    }

    void CrashDumpWriterVFS::writeWords(const uint32_t *buff, std::size_t size)
    {
        if (std::fwrite(buff, sizeof(*buff), size, file) != size) {
            LOG_FATAL("Unable to write crash dump errno: %i", errno);
            _exit_backtrace(-1, false);
        }
    }

} // namespace crashdump
