// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "crashdumpwriter_vfs.hpp"

#include <log.hpp>
#include <fcntl.h>
#include "purefs/vfs_subsystem.hpp"

namespace crashdump
{
    void CrashDumpWriterVFS::openDump()
    {
        std::array<char, 64> crashDumpFileName;
        formatCrashDumpFileName(crashDumpFileName);

        vfs    = purefs::subsystem::vfs_core();
        dumpFd = vfs->open(crashDumpFileName.data(), O_WRONLY | O_CREAT | O_TRUNC, 0666);

        if (dumpFd < 0) {
            LOG_FATAL("Failed to open crash dump file [%s]. Won't be able to save crash info.",
                      crashDumpFileName.data());
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
