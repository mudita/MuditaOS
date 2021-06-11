// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BackupRestore.hpp"

#include <SystemManager/SystemManager.hpp>
#include <log.hpp>
#include <microtar.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/DBServiceName.hpp>
#include <module-utils/Utils.hpp>

#include <cassert>
#include <filesystem>
#include <purefs/filesystem_paths.hpp>
#include <memory>
#include <string>
#include <vector>
#include <sys/statvfs.h>
#include <gsl/gsl>

namespace sys
{
    class Service;
} // namespace sys

static const long unsigned int empty_dirlist_size = 2;
static bool isValidDirentry(const std::filesystem::directory_entry &direntry)
{
    return direntry.path() != "." && direntry.path() != ".." && direntry.path() != "...";
}
// this replaces std::filesystem::copy_file that is broken at
// the time this is implemented (chown issues after copy)
// once fixed this can be replaced
static bool copyFile(const std::filesystem::path &from, const std::filesystem::path &to)
{
    std::FILE *fromFp = std::fopen(from.c_str(), "r");
    std::FILE *toFp   = std::fopen(to.c_str(), "w");

    auto end = gsl::finally([fromFp, toFp] {
        std::fclose(fromFp);
        std::fclose(toFp);
    });

    if (fromFp == nullptr) {
        LOG_ERROR("can't open %s for reading", from.c_str());
        return false;
    }

    if (toFp == nullptr) {
        LOG_ERROR("can't open %s for writing", to.c_str());
        return false;
    }

    std::unique_ptr<unsigned char[]> buffer = std::make_unique<unsigned char[]>(purefs::buffer::tar_buf);
    size_t bytes;

    while ((bytes = std::fread(buffer.get(), 1, purefs::buffer::tar_buf, fromFp)) != 0) {
        if (std::fwrite(buffer.get(), 1, bytes, toFp) != bytes) {
            return false;
        }
    }

    return true;
}

bool BackupRestore::BackupUserFiles(sys::Service *ownerService, std::filesystem::path &path)
{
    assert(ownerService != nullptr);
    LOG_INFO("BackupUserFiles: backup started...");

    if (BackupRestore::RemoveBackupDir(path) == false) {
        return false;
    }

    if (BackupRestore::CreateBackupDir(path) == false) {
        return false;
    }

    LOG_INFO("BackupUserFiles: database backup started...");

    if (DBServiceAPI::DBBackup(ownerService, path) == false) {
        LOG_ERROR("BackupUserFiles: database backup failed, quitting...");
        BackupRestore::RemoveBackupDir(path);
        return false;
    }

    if (WriteBackupInfo(ownerService, path) == false) {
        LOG_ERROR("Failed to write backup info");
        BackupRestore::RemoveBackupDir(path);
        return false;
    }

    LOG_INFO("BackupUserFiles: packing files");
    return BackupRestore::PackUserFiles(path);
}

bool BackupRestore::WriteBackupInfo(sys::Service *ownerService, const std::filesystem::path &path)
{
    LOG_INFO("Writing backup info to %s", path.c_str());

    if (std::filesystem::is_directory(path)) {
        try {
            copyFile(purefs::dir::getRootDiskPath() / purefs::file::boot_json, path / bkp::backupInfo);

            LOG_DEBUG("%s copied to %s",
                      (purefs::dir::getRootDiskPath() / purefs::file::boot_json).c_str(),
                      (path / bkp::backupInfo).c_str());

            return true;
        }
        catch (std::filesystem::filesystem_error &e) {
            LOG_ERROR("failed to copy %s->%s error:\"%s\"",
                      (purefs::dir::getRootDiskPath() / purefs::file::boot_json).c_str(),
                      (path / bkp::backupInfo).c_str(),
                      e.what());

            return false;
        }
    }
    else {
        LOG_ERROR("%s is not a directory", path.c_str());
        return false;
    }

    return true;
}

bool BackupRestore::RestoreUserFiles(sys::Service *ownerService, const std::filesystem::path &path)
{
    assert(ownerService != nullptr);
    LOG_INFO("RestoreUserFiles: restore started from %s", path.c_str());

    if (BackupRestore::UnpackBackupFile(path) == false) {
        return false;
    }

    if (sys::SystemManager::Restore(ownerService) == false) {
        LOG_ERROR("Can't enter update system state");
        return false;
    }
    else {
        LOG_INFO("entered update state");

        if (BackupRestore::ReplaceUserFiles(path) == false) {
            LOG_ERROR("can't restore user files");
            return false;
        }
    }

    // BackupRestore::RemoveBackupDir(path);

    return true;
}

bool BackupRestore::RemoveBackupDir(std::filesystem::path &path)
{
    /* prepare directories */
    if (std::filesystem::is_directory(path)) {
        LOG_INFO("RemoveBackupDir: removing backup directory %s...", path.c_str());

        try {
            std::filesystem::remove_all(path);
        }
        catch (const std::filesystem::filesystem_error &e) {
            LOG_ERROR("RemoveBackupDir: removing backup directory %s failed, error: %s.", path.c_str(), e.what());
            return false;
        }
    }

    return true;
}

bool BackupRestore::CreateBackupDir(std::filesystem::path &path)
{
    LOG_INFO("CreateBackupDir: creating backup directory %s...", path.c_str());
    std::error_code errorCode;

    if (!std::filesystem::is_directory(purefs::dir::getBackupOSPath())) {
        if (!std::filesystem::create_directory(purefs::dir::getBackupOSPath(), errorCode)) {
            LOG_ERROR("CreateBackupDir: creating backup directory %s failed. \"%s\"",
                      purefs::dir::getBackupOSPath().c_str(),
                      errorCode.message().c_str());
            return false;
        }
    }

    if (!std::filesystem::is_directory(purefs::dir::getTemporaryPath())) {
        if (!std::filesystem::create_directory(purefs::dir::getTemporaryPath(), errorCode)) {
            LOG_ERROR("CreateBackupDir: creating backup directory %s failed. \"%s\"",
                      purefs::dir::getTemporaryPath().c_str(),
                      errorCode.message().c_str());
            return false;
        }
    }

    if (!std::filesystem::is_directory(path)) {
        if (!std::filesystem::create_directory(path, errorCode)) {
            LOG_ERROR("CreateBackupDir: creating backup directory %s failed. \"%s\"",
                      path.c_str(),
                      errorCode.message().c_str());
            return false;
        }
    }

    return true;
}

bool BackupRestore::PackUserFiles(std::filesystem::path &path)
{
    if (std::filesystem::is_empty(path)) {
        LOG_ERROR("backup dir %s is empty, nothing to backup, quitting...", path.c_str());
        BackupRestore::RemoveBackupDir(path);
        return false;
    }

    std::filesystem::path tarFilePath =
        (purefs::dir::getBackupOSPath() / path.filename()).replace_extension(purefs::extension::tar);
    mtar_t tarFile;

    LOG_INFO("opening file %s...", tarFilePath.c_str());

    int ret = mtar_open(&tarFile, tarFilePath.c_str(), "w");

    if (ret != MTAR_ESUCCESS) {
        LOG_ERROR("opening file %s failed, error: %s, quitting...", tarFilePath.c_str(), mtar_strerror(ret));
        BackupRestore::RemoveBackupDir(path);
        return false;
    }

    auto buffer = std::make_unique<unsigned char[]>(purefs::buffer::tar_buf);
    std::error_code e;

    for (auto &direntry : std::filesystem::directory_iterator(path)) {
        if (!isValidDirentry(direntry)) {
            continue;
        }

        LOG_INFO("archiving file %s...", direntry.path().c_str());
        auto *file = std::fopen(direntry.path().string().c_str(), "r");

        if (file == nullptr) {
            LOG_ERROR("archiving file %s failed, cannot open file, quitting...", direntry.path().c_str());
            mtar_close(&tarFile);
            BackupRestore::RemoveBackupDir(path);
            return false;
        }

        LOG_DEBUG("writting tar header for %s...", direntry.path().c_str());

        if (mtar_write_file_header(&tarFile,
                                   direntry.path().filename().c_str(),
                                   static_cast<unsigned>(std::filesystem::file_size(direntry))) != MTAR_ESUCCESS) {
            LOG_ERROR("writing tar header for %s failed", direntry.path().filename().c_str());
            std::fclose(file);
            mtar_close(&tarFile);
            BackupRestore::RemoveBackupDir(path);
            return false;
        }

        uintmax_t filesize = std::filesystem::file_size(direntry.path(), e);
        if (e) {
            LOG_ERROR("failed to get size for file: %s \"%s\"", path.c_str(), e.message().c_str());
            BackupRestore::RemoveBackupDir(path);
            return false;
        }
        uint32_t loopcount = (filesize / purefs::buffer::tar_buf) + 1u;
        uint32_t readsize;

        for (uint32_t i = 0u; i < loopcount; i++) {
            if (i + 1u == loopcount) {
                readsize = filesize % purefs::buffer::tar_buf;
            }
            else {
                readsize = purefs::buffer::tar_buf;
            }

            LOG_DEBUG("reading file %s...", direntry.path().c_str());

            if (std::fread(buffer.get(), 1, readsize, file) != readsize) {
                LOG_ERROR("reading file %s failed, quitting...", direntry.path().c_str());
                std::fclose(file);
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir(path);
                return false;
            }

            LOG_DEBUG("writting %s into backup...", direntry.path().c_str());
            if (mtar_write_data(&tarFile, buffer.get(), readsize) != MTAR_ESUCCESS) {
                LOG_ERROR("PackUserFiles: writting %s into backup failed, quitting...", direntry.path().c_str());
                std::fclose(file);
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir(path);
                return false;
            }
        }

        LOG_INFO("closing file %s...", direntry.path().c_str());
        if (std::fclose(file) != 0) {
            LOG_ERROR("PackUserFiles: closing file %s failed, quitting...", direntry.path().c_str());
            mtar_close(&tarFile);
            BackupRestore::RemoveBackupDir(path);
            return false;
        }

        LOG_INFO("deleting file %s...", direntry.path().c_str());

        if (std::remove(direntry.path().c_str()) != 0) {
            LOG_ERROR("PackUserFiles: deleting file %s failed, quitting...", direntry.path().c_str());
            mtar_close(&tarFile);
            BackupRestore::RemoveBackupDir(path);
            return false;
        }
    }

    LOG_INFO("finalizing file %s...", tarFilePath.c_str());
    if (mtar_finalize(&tarFile) != MTAR_ESUCCESS) {
        LOG_ERROR("PackUserFiles: finalizing file %s failed, quitting....", tarFilePath.c_str());
        mtar_close(&tarFile);
        BackupRestore::RemoveBackupDir(path);
        return false;
    }

    LOG_INFO("closing file %s...", tarFilePath.c_str());
    if (mtar_close(&tarFile) != MTAR_ESUCCESS) {
        LOG_ERROR("PackUserFiles: closing file %s failed, quitting...", tarFilePath.c_str());
        BackupRestore::RemoveBackupDir(path);
        return false;
    }

    return true;
}

bool BackupRestore::UnpackBackupFile(const std::filesystem::path &tarFilePath)
{
    mtar_t tarFile;
    mtar_header_t tarHeader;
    std::error_code e;

    auto extractDestination = purefs::dir::getTemporaryPath() / tarFilePath.stem();

    LOG_INFO("creating temporary directory %s", extractDestination.c_str());
    if (!std::filesystem::is_directory(extractDestination, e)) {
        std::filesystem::create_directory(extractDestination, e);
        if (e) {
            LOG_ERROR("Can't create temporary directory %s \"%s\"", extractDestination.c_str(), e.message().c_str());
            return false;
        }
    }

    LOG_INFO("opening file %s...", tarFilePath.c_str());

    int ret = mtar_open(&tarFile, tarFilePath.c_str(), "r");

    if (ret != MTAR_ESUCCESS) {
        LOG_ERROR("opening file %s failed, error: %s, quitting...", tarFilePath.c_str(), mtar_strerror(ret));
        return false;
    }

    auto buffer = std::make_unique<unsigned char[]>(purefs::buffer::tar_buf);

    do {
        ret = mtar_read_header(&tarFile, &tarHeader);
        LOG_DEBUG("reading tar header name %s...", tarHeader.name);

        if ((tarHeader.type == MTAR_TREG) && (ret == MTAR_ESUCCESS)) {
            LOG_DEBUG("extracting file %s...", tarHeader.name);

            std::filesystem::path extractedFile = extractDestination / tarHeader.name;
            auto *file                          = std::fopen(extractedFile.c_str(), "w");

            if (file == nullptr) {
                LOG_ERROR("can't open %s for writing", extractedFile.c_str());
                mtar_close(&tarFile);
                return false;
            }

            uint32_t loopcount = (tarHeader.size / purefs::buffer::tar_buf) + 1u;
            uint32_t readsize  = 0u;

            for (uint32_t i = 0u; i < loopcount; i++) {

                if (i + 1u == loopcount) {
                    readsize = tarHeader.size % purefs::buffer::tar_buf;
                }
                else {
                    readsize = purefs::buffer::tar_buf;
                }

                if (mtar_read_data(&tarFile, buffer.get(), readsize) != MTAR_ESUCCESS) {
                    LOG_ERROR("extracting file %s failed, quitting...", extractedFile.c_str());
                    mtar_close(&tarFile);
                    std::fclose(file);
                    std::remove(extractedFile.c_str());
                    return false;
                }

                if (std::fwrite(buffer.get(), 1, readsize, file) != readsize) {
                    LOG_ERROR("writting file %s failed, quitting...", extractedFile.c_str());
                    mtar_close(&tarFile);
                    std::fclose(file);
                    std::remove(extractedFile.c_str());
                    return false;
                }
            }

            LOG_INFO("extracting file %s succeeded", extractedFile.c_str());
            std::fclose(file);
        }
        else {
            LOG_DEBUG("found header %d, skipping", tarHeader.type);
        }

        ret = mtar_next(&tarFile);
        LOG_DEBUG("reading tar next status %s", mtar_strerror(ret));
    } while (ret == MTAR_ESUCCESS);

    LOG_DEBUG("cleanup %s", tarFilePath.c_str());
    mtar_close(&tarFile);
    std::remove(tarFilePath.c_str());

    if (e) {
        LOG_WARN("can't cleanup temporary dir %s \"%s\"", extractDestination.c_str(), e.message().c_str());
    }

    return true;
}

bool BackupRestore::ReplaceUserFiles(const std::filesystem::path &path)
{
    /* replace existing files that have respective backup files existing */
    const auto tempDir = purefs::dir::getTemporaryPath() / path.stem();

    if (std::filesystem::is_directory(tempDir) && std::filesystem::is_empty(tempDir)) {
        LOG_INFO("dir empty, nothing to restore, quitting...");
        return false;
    }

    const std::filesystem::path userDir   = purefs::dir::getUserDiskPath();
    const std::filesystem::path backupDir = purefs::dir::getBackupOSPath();
    std::error_code e;

    for (auto &direntry : std::filesystem::directory_iterator(tempDir, e)) {
        if (e) {
            LOG_INFO("Can't list contents of %s \"%s\"", tempDir.c_str(), e.message().c_str());
            return false;
        }

        if (!isValidDirentry(direntry)) {
            continue;
        }

        // dont restore the information file
        if (direntry.path().filename() == bkp::backupInfo) {
            continue;
        }

        LOG_INFO("restoring backup file %s...", direntry.path().c_str());

        if (std::filesystem::remove(userDir / direntry.path().filename(), e)) {
            std::filesystem::rename(tempDir / direntry.path().filename(), userDir / direntry.path().filename(), e);
            if (e) {
                LOG_ERROR("can't rename %s->%s restore failed \"%s\"",
                          (tempDir / direntry.path().filename()).c_str(),
                          (userDir / direntry.path().filename()).c_str(),
                          e.message().c_str());
                return false;
            }
            else {
                LOG_INFO("restored %s->%s",
                         (tempDir / direntry.path().filename()).c_str(),
                         (userDir / direntry.path().filename()).c_str());
            }
        }
        else {
            LOG_WARN("can't remove %s \"%s\"", (userDir / direntry.path().filename()).c_str(), e.message().c_str());
            // we should continue, there can be new files in the backup
        }
    }

    return true;
}

json11::Json BackupRestore::GetBackupFiles()
{
    auto dirEntryVector = std::vector<std::string>();
    std::error_code e;
    for (const auto &p : std::filesystem::directory_iterator(purefs::dir::getBackupOSPath(), e)) {
        if (e) {
            LOG_ERROR(
                "Can't get directory %s contents \"%s\"", purefs::dir::getBackupOSPath().c_str(), e.message().c_str());
            return json11::Json();
        }
        if (!p.is_directory() && p.path().extension() == purefs::extension::tar) {
            LOG_DEBUG("possible restore file %s", p.path().filename().c_str());
            dirEntryVector.push_back(p.path().filename());
        }
    }

    return dirEntryVector;
}
