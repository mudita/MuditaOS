// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-desktop/Sync.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <purefs/filesystem_paths.hpp>

#include <fstream>
#include <gsl/util>
#include <microtar.hpp>

namespace sys
{
    class Service;
} // namespace sys

static bool isValidDirentry(const std::filesystem::directory_entry &direntry)
{
    return direntry.path() != "." && direntry.path() != ".." && direntry.path() != "...";
}

Sync::CompletionCode Sync::PrepareSyncPackage(sys::Service *ownerService, std::filesystem::path &path)
{
    assert(ownerService != nullptr);
    LOG_INFO("Sync package preparation started...");

    if (!Sync::RemoveSyncDir(path)) {
        return CompletionCode::FSError;
    }

    if (!Sync::CreateSyncDir(path)) {
        return CompletionCode::FSError;
    }

    LOG_DEBUG("Sync package preparation started...");
    if (!DBServiceAPI::DBPrepareSyncPackage(ownerService, path)) {
        LOG_ERROR("Sync package preparation, quitting...");
        Sync::RemoveSyncDir(path);
        return CompletionCode::DBError;
    }

    LOG_DEBUG("Packing files");
    if (!Sync::PackSyncFiles(path)) {
        LOG_ERROR("Failed pack sync files");
        Sync::RemoveSyncDir(path);
        return CompletionCode::PackError;
    }

    if (!Sync::RemoveSyncDir(path)) {
        return CompletionCode::FSError;
    }

    return CompletionCode::Success;
}

bool Sync::RemoveSyncDir(const std::filesystem::path &path)
{
    /* prepare directories */
    if (std::filesystem::is_directory(path)) {
        LOG_INFO("Removing sync directory %s...", path.c_str());
        std::error_code errorCode;

        if (std::filesystem::remove_all(path, errorCode) == 0) {
            LOG_ERROR("Removing sync directory %s failed, error: %d.", path.c_str(), errorCode.value());
            return false;
        }
    }

    return true;
}

bool Sync::CreateSyncDir(const std::filesystem::path &path)
{
    LOG_INFO("Creating sync directory %s...", path.c_str());
    if (std::filesystem::exists(path)) {
        LOG_ERROR("Sync directory already exists!");
        return false;
    }
    std::error_code errorCode;
    if (!std::filesystem::create_directories(path, errorCode)) {
        LOG_ERROR("Failed to create directory: %s, error: %d", path.c_str(), errorCode.value());
        return false;
    }
    return true;
}

bool Sync::PackSyncFiles(const std::filesystem::path &path)
{
    if (std::filesystem::is_empty(path)) {
        LOG_ERROR("Sync dir is empty, quitting...");
        return false;
    }

    auto isTarFileOpen                = false;
    std::filesystem::path tarFilePath = (purefs::dir::getSyncPackagePath() / path.filename());
    mtar_t tarFile;

    auto cleanup = gsl::finally([&isTarFileOpen, &tarFile]() {
        if (isTarFileOpen) {
            mtar_close(&tarFile);
        }
    });

    LOG_INFO("Opening tar %s file...", tarFilePath.c_str());

    int ret = mtar_open(&tarFile, tarFilePath.c_str(), "w");
    if (ret != MTAR_ESUCCESS) {
        LOG_ERROR("Opening tar file failed, quitting...");
        return false;
    }
    isTarFileOpen = true;

    auto fileStreamBuffer = std::make_unique<char[]>(purefs::buffer::tar_buf);
    auto tarStreamBuffer  = std::make_unique<char[]>(purefs::buffer::tar_buf);
    setvbuf(tarFile.stream, tarStreamBuffer.get(), _IOFBF, purefs::buffer::tar_buf);
    std::error_code errorCode;

    for (const auto &direntry : std::filesystem::directory_iterator(path)) {
        if (!isValidDirentry(direntry)) {
            continue;
        }

        LOG_INFO("Archiving file ...");
        LOG_DEBUG("Writing tar header ...");

        if (mtar_write_file_header(&tarFile,
                                   direntry.path().filename().c_str(),
                                   static_cast<unsigned>(std::filesystem::file_size(direntry))) != MTAR_ESUCCESS) {
            LOG_ERROR("Writing tar header failed");
            return false;
        }

        const auto filesize = std::filesystem::file_size(direntry.path(), errorCode);
        if (errorCode) {
            LOG_ERROR("Failed to get size for file %s, error: %d", direntry.path().c_str(), errorCode.value());
            return false;
        }
        const auto loopCount = (filesize / purefs::buffer::tar_buf) + 1u;
        std::ifstream fileStream(direntry.path().string().c_str());

        for (auto i = 0u; i < loopCount; i++) {
            if (fileStream.bad()) {
                LOG_ERROR("File stream error.");
                return false;
            }
            uint32_t readSize;
            if (i + 1u == loopCount) {
                readSize = filesize % purefs::buffer::tar_buf;
            }
            else {
                readSize = purefs::buffer::tar_buf;
            }

            LOG_DEBUG("Reading file ...");
            fileStream.read(fileStreamBuffer.get(), readSize);

            LOG_DEBUG("Writing into sync package...");
            if (mtar_write_data(&tarFile, fileStreamBuffer.get(), readSize) != MTAR_ESUCCESS) {
                LOG_ERROR("Writing into sync package failed, quitting...");
                return false;
            }
        }
    }

    LOG_INFO("Finalizing tar file...");
    if (mtar_finalize(&tarFile) != MTAR_ESUCCESS) {
        LOG_ERROR("Finalizing tar file failed, quitting....");
        return false;
    }

    LOG_INFO("Closing tar file...");
    if (mtar_close(&tarFile) != MTAR_ESUCCESS) {
        LOG_ERROR("Closing tar file failed, quitting...");
        return false;
    }
    isTarFileOpen = false;

    return true;
}
