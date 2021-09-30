// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "crashdumpwriter_vfs.hpp"

#include <log/log.hpp>
#include <fcntl.h>
#include "purefs/vfs_subsystem.hpp"
#include <purefs/filesystem_paths.hpp>

#include <filesystem>

namespace crashdump
{
    constexpr inline auto crashDumpFileName = "crashdump.hex";

    void CrashDumpWriterVFS::openDump()
    {
        vfs                          = purefs::subsystem::vfs_core();
        const auto crashDumpFilePath = purefs::dir::getCrashDumpsPath() / crashDumpFileName;

        if (!rotator.rotateFile(crashDumpFilePath)) {
            LOG_FATAL("Failed to rotate crash dumps.");
        }
        dumpFd = vfs->open(crashDumpFilePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);

        if (dumpFd < 0) {
            LOG_FATAL("Failed to open crash dump file. Won't be able to save crash info.");
        }
    }

    void CrashDumpWriterVFS::saveDump()
    {
        vfs->close(dumpFd);
    }

    void CrashDumpWriterVFS::writeBytes(const uint8_t *buff, std::size_t size)
    {
        vfs->write(dumpFd, reinterpret_cast<const char *>(buff), size);
    }

    void CrashDumpWriterVFS::writeHalfWords(const uint16_t *buff, std::size_t size)
    {
        for (std::size_t n = 0; n < size; ++n) {
            writeBytes(reinterpret_cast<const uint8_t *>(buff + n), sizeof(uint16_t));
        }
    }

    void CrashDumpWriterVFS::writeWords(const uint32_t *buff, std::size_t size)
    {
        for (std::size_t n = 0; n < size; ++n) {
            writeBytes(reinterpret_cast<const uint8_t *>(buff + n), sizeof(uint32_t));
        }
    }

    CrashDumpWriter &CrashDumpWriter::instance()
    {
        static CrashDumpWriterVFS writer;
        return writer;
    }

} // namespace crashdump
