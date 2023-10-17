// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "crashdumpwriter_vfs.hpp"
#include <cstdio>
#include <fcntl.h>
#include <purefs/filesystem_paths.hpp>
#include <exit_backtrace.h>
#include <CrashdumpMetadataStore.hpp>

#include <log/log.hpp>
#include <filesystem>
#include <unistd.h>

/* Crashdump filename pattern:
 * crashdump_[os-metadata]_[timestamp-in-seconds].hex.[index]
 * where [os-metadata] is in [product]_[os-version]_[commit-hash]-[serial-number] form.
 * [index] was added to ensure resistance to the device date retraction.
 */

namespace
{
    inline constexpr auto prefix     = "crashdump";
    inline constexpr auto extension  = ".hex";
    inline constexpr auto file_index = ".1";
    inline constexpr auto separator  = "_";

    inline std::string generateCrashdumpFilename()
    {
        const auto crashTime   = std::time(nullptr);
        const auto &osMetadata = Store::CrashdumpMetadata::getInstance().getMetadataString();
        return (std::string(prefix) + separator + osMetadata + separator + std::to_string(crashTime) + extension +
                file_index);
    }
} // namespace

namespace crashdump
{
    void CrashDumpWriterVFS::openDump()
    {
        const auto crashdumpFilePath = purefs::dir::getCrashDumpsPath() / generateCrashdumpFilename();

        LOG_INFO("Preparing crashdump '%s'...", crashdumpFilePath.c_str());
        if (!rotator.rotateFiles(purefs::dir::getCrashDumpsPath())) {
            LOG_FATAL("Failed to rotate crashdump, errno: %d", errno);
            _exit_backtrace(-1, false);
        }

        file = std::fopen(crashdumpFilePath.c_str(), "w");
        if (file == nullptr) {
            LOG_FATAL("Failed to open crashdump file '%s', errno: %d", crashdumpFilePath.c_str(), errno);
            _exit_backtrace(-1, false);
        }
    }

    void CrashDumpWriterVFS::saveDump()
    {
        LOG_INFO("Crashdump created successfully!");
        std::fflush(file);
        fsync(fileno(file));
        std::fclose(file);
    }

    void CrashDumpWriterVFS::writeBytes(const std::uint8_t *buff, std::size_t size)
    {
        if (std::fwrite(buff, sizeof(*buff), size, file) != size) {
            LOG_FATAL("Unable to write crashdump, errno: %d", errno);
            _exit_backtrace(-1, false);
        }
    }

    void CrashDumpWriterVFS::writeHalfWords(const std::uint16_t *buff, std::size_t size)
    {
        if (std::fwrite(buff, sizeof(*buff), size, file) != size) {
            LOG_FATAL("Unable to write crashdump, errno: %d", errno);
            _exit_backtrace(-1, false);
        }
    }

    void CrashDumpWriterVFS::writeWords(const std::uint32_t *buff, std::size_t size)
    {
        if (std::fwrite(buff, sizeof(*buff), size, file) != size) {
            LOG_FATAL("Unable to write crashdump, errno: %d", errno);
            _exit_backtrace(-1, false);
        }
    }
} // namespace crashdump
