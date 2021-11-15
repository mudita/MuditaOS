// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "crashdumpwriter_vfs.hpp"

#include <cstdio>
#include <log/log.hpp>
#include <fcntl.h>
#include "purefs/vfs_subsystem.hpp"
#include <purefs/filesystem_paths.hpp>
#include <exit_backtrace.h>

#include <filesystem>
#include <stdint.h>
#include <unistd.h>

namespace crashdump
{
    constexpr inline auto crashDumpFileName = "crashdump.hex";

    void CrashDumpWriterVFS::openDump()
    {
        const auto crashDumpFilePath = purefs::dir::getCrashDumpsPath() / crashDumpFileName;
        LOG_INFO("Crash dump %s preparing ...", crashDumpFilePath.c_str());
        if (!rotator.rotateFile(crashDumpFilePath)) {
            LOG_FATAL("Failed to rotate crash dumps errno: %i", errno);
            _exit_backtrace(-1, false);
        }
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
