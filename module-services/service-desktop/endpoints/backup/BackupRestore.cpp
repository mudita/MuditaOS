// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BackupRestore.hpp"

#include <SystemManager/SystemManager.hpp>
#include <log/log.hpp>
#include <microtar/src/microtar.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/DBServiceName.hpp>
#include <module-utils/Utils.hpp>

#include <cassert>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <vfs.hpp>

namespace sys
{
    class Service;
} // namespace sys

static const long unsigned int empty_dirlist_size = 2;
static const auto backup_file_name                = "backup.tar";

void BackupRestore::BackupUserFiles(sys::Service *ownerService)
{
    assert(ownerService != nullptr);

    LOG_INFO("BackupUserFiles: backup started...");

    if (BackupRestore::RemoveBackupDir() == false) {
        return;
    }

    if (BackupRestore::CreateBackupDir() == false) {
        return;
    }

    std::string backupPathDB = PATH_BACKUP;
    backupPathDB += "/";

    LOG_INFO("BackupUserFiles: database backup started...");

    if (DBServiceAPI::DBBackup(ownerService, backupPathDB) == false) {
        LOG_ERROR("BackupUserFiles: database backup failed, quitting...");
        BackupRestore::RemoveBackupDir();
        return;
    }

    BackupRestore::PackUserFiles();

    LOG_INFO("BackupUserFiles: backup finished");
}

void BackupRestore::RestoreUserFiles(sys::Service *ownerService)
{
    assert(ownerService != nullptr);

    LOG_INFO("RestoreUserFiles: restore started...");

    if (BackupRestore::UnpackBackupFile() == false) {
        return;
    }

    /* close user files to be restored */
    LOG_INFO("RestoreUserFiles: closing ServiceDB...");
    std::string dbServiceName = service::name::db;
    sys::SystemManager::DestroyService(dbServiceName, ownerService);

    BackupRestore::ReplaceUserFiles();

    BackupRestore::RemoveBackupDir();

    LOG_INFO("RestoreUserFiles: restoring finished, rebooting...");
    sys::SystemManager::Reboot(ownerService);
}

bool BackupRestore::RemoveBackupDir()
{
    /* prepare directories */
    if (const auto backupOSPath = purefs::dir::getBackupOSPath(); std::filesystem::is_directory(backupOSPath.c_str())) {
        LOG_INFO("RemoveBackupDir: removing backup directory %s...", backupOSPath.c_str());

        try {
            std::filesystem::remove_all(backupOSPath.c_str());
        }
        catch (const std::filesystem::filesystem_error &e) {
            LOG_ERROR(
                "RemoveBackupDir: removing backup directory %s failed, error: %s.", backupOSPath.c_str(), e.what());
            return false;
        }
    }

    return true;
}

bool BackupRestore::CreateBackupDir()
{
    const auto backupOSPath = purefs::dir::getBackupOSPath();
    LOG_INFO("CreateBackupDir: creating backup directory %s...", backupOSPath.c_str());

    if (!std::filesystem::is_directory(backupOSPath.c_str())) {
        if (!std::filesystem::create_directory(backupOSPath.c_str())) {
            LOG_ERROR("CreateBackupDir: creating backup directory %s failed.", backupOSPath.c_str());
            return false;
        }
    }

    return true;
}

bool BackupRestore::PackUserFiles()
{
    std::string backupPathDB = PATH_BACKUP;
    backupPathDB += "/";

    const auto backupOSPath = purefs::dir::getBackupOSPath();

    if (std::filesystem::is_empty(backupOSPath.c_str())) {
        LOG_ERROR("PackUserFiles: backup dir %s is empty, nothing to backup, quitting...", backupOSPath.c_str());
        BackupRestore::RemoveBackupDir();
        return false;
    }

    std::string tarFilePath = backupOSPath;
    tarFilePath += "/";
    tarFilePath += backup_file_name;

    mtar_t tarFile;

    LOG_INFO("PackUserFiles: opening file %s...", tarFilePath.c_str());

    int ret = mtar_open(&tarFile, tarFilePath.c_str(), "w");

    if (ret != MTAR_ESUCCESS) {
        LOG_ERROR(
            "PackUserFiles: opening file %s failed, error: %s, quitting...", tarFilePath.c_str(), mtar_strerror(ret));
        BackupRestore::RemoveBackupDir();
        return false;
    }

    std::unique_ptr<unsigned char[]> buffer(new unsigned char[purefs::buffer::tar_buf]);

    for (auto &direntry : std::filesystem::directory_iterator(backupOSPath.c_str())) {
        if ((direntry.path().string().compare(".") != 0) && (direntry.path().string().compare("..") != 0) &&
            (direntry.path().string().compare("...") != 0)) {

            LOG_INFO("PackUserFiles: archiving file %s...", (backupPathDB + direntry.path().string()).c_str());
            auto *file = std::fopen((backupPathDB + direntry.path().string()).c_str(), "r");

            if (file == nullptr) {
                LOG_ERROR("PackUserFiles: archiving file %s failed, cannot open file, quitting...",
                          direntry.path().string().c_str());
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir();
                return false;
            }

            LOG_INFO("PackUserFiles: writting tar header for %s...", direntry.path().string().c_str());

            if (mtar_write_file_header(&tarFile,
                                       direntry.path().string().c_str(),
                                       static_cast<unsigned>(std::filesystem::file_size(direntry))) != MTAR_ESUCCESS) {
                LOG_ERROR("PackUserFiles: writing tar header for %s failed", direntry.path().string().c_str());
                std::fclose(file);
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir();
                return false;
            }

            uint32_t loopcount = (utils::filesystem::filelength(file) / purefs::buffer::tar_buf) + 1u;
            uint32_t readsize  = 0u;

            for (uint32_t i = 0u; i < loopcount; i++) {
                if (i + 1u == loopcount) {
                    readsize = utils::filesystem::filelength(file) % purefs::buffer::tar_buf;
                }
                else {
                    readsize = purefs::buffer::tar_buf;
                }

                LOG_INFO("PackUserFiles: reading file %s...", direntry.path().string().c_str());

                if (std::fread(buffer.get(), 1, readsize, file) != readsize) {
                    LOG_ERROR("PackUserFiles: reading file %s failed, quitting...", direntry.path().string().c_str());
                    std::fclose(file);
                    mtar_close(&tarFile);
                    BackupRestore::RemoveBackupDir();
                    return false;
                }

                LOG_INFO("PackUserFiles: writting %s into backup...", direntry.path().string().c_str());
                if (mtar_write_data(&tarFile, buffer.get(), readsize) != MTAR_ESUCCESS) {
                    LOG_ERROR("PackUserFiles: writting %s into backup failed, quitting...",
                              direntry.path().string().c_str());
                    std::fclose(file);
                    mtar_close(&tarFile);
                    BackupRestore::RemoveBackupDir();
                    return false;
                }
            }

            LOG_INFO("PackUserFiles: closing file %s...", (backupPathDB + direntry.path().string()).c_str());
            if (std::fclose(file) != 0) {
                LOG_ERROR("PackUserFiles: closing file %s failed, quitting...",
                          (backupPathDB + direntry.path().string()).c_str());
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir();
                return false;
            }

            LOG_INFO("PackUserFiles: deleting file %s...", (backupPathDB + direntry.path().string()).c_str());
            if (std::remove((backupPathDB + direntry.path().string()).c_str()) != 0) {
                LOG_ERROR("PackUserFiles: deleting file %s failed, quitting...",
                          (backupPathDB + direntry.path().string()).c_str());
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir();
                return false;
            }
        }
    }

    LOG_INFO("PackUserFiles: finalizing file %s...", tarFilePath.c_str());
    if (mtar_finalize(&tarFile) != MTAR_ESUCCESS) {
        LOG_ERROR("PackUserFiles: finalizing file %s failed, quitting....", tarFilePath.c_str());
        mtar_close(&tarFile);
        BackupRestore::RemoveBackupDir();
        return false;
    }

    LOG_INFO("PackUserFiles: closing file %s...", tarFilePath.c_str());
    if (mtar_close(&tarFile) != MTAR_ESUCCESS) {
        LOG_ERROR("PackUserFiles: closing file %s failed, quitting...", tarFilePath.c_str());
        BackupRestore::RemoveBackupDir();
        return false;
    }

    return true;
}

bool BackupRestore::UnpackBackupFile()
{
    std::string tarFilePath = purefs::dir::getBackupOSPath();
    tarFilePath += "/";
    tarFilePath += backup_file_name;

    mtar_t tarFile;
    mtar_header_t tarHeader;

    LOG_INFO("UnpackBackupFile: opening file %s...", tarFilePath.c_str());

    int ret = mtar_open(&tarFile, tarFilePath.c_str(), "r");

    if (ret != MTAR_ESUCCESS) {
        LOG_ERROR("UnpackBackupFile: opening file %s failed, error: %s, quitting...",
                  tarFilePath.c_str(),
                  mtar_strerror(ret));
        return false;
    }

    std::unique_ptr<unsigned char[]> buffer(new unsigned char[purefs::buffer::tar_buf]);

    do {
        ret = mtar_read_header(&tarFile, &tarHeader);
        LOG_INFO("UnpackBackupFile: reading tar header name %s...", tarHeader.name);

        if ((tarHeader.type == MTAR_TREG) && (ret == MTAR_ESUCCESS)) {
            LOG_INFO("UnpackBackupFile: extracting file %s...", tarHeader.name);

            std::string restoreFilePath = purefs::dir::getBackupOSPath();
            restoreFilePath += "/";
            restoreFilePath += tarHeader.name;
            auto *file = std::fopen(restoreFilePath.c_str(), "w");

            if (file == nullptr) {
                LOG_ERROR("UnpackBackupFile: extracting file %s failed, quitting...", tarHeader.name);
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
                    LOG_ERROR("UnpackBackupFile: extracting file %s failed, quitting...", tarHeader.name);
                    mtar_close(&tarFile);
                    std::fclose(file);
                    std::remove(restoreFilePath.c_str());
                    return false;
                }

                if (std::fwrite(buffer.get(), 1, readsize, file) != readsize) {
                    LOG_ERROR("UnpackBackupFile: writting file %s failed, quitting...", restoreFilePath.c_str());
                    mtar_close(&tarFile);
                    std::fclose(file);
                    std::remove(restoreFilePath.c_str());
                    return false;
                }
            }

            LOG_INFO("UnpackBackupFile: extracting file %s succeeded", tarHeader.name);
            std::fclose(file);
        }
        else {
            LOG_INFO("UnpackBackupFile: found header %d, skipping", tarHeader.type);
        }

        ret = mtar_next(&tarFile);
        LOG_INFO("UnpackBackupFile: reading tar next status %s", mtar_strerror(ret));
    } while (ret == MTAR_ESUCCESS);

    LOG_INFO("UnpackBackupFile: cleaning directory from tar file...");
    mtar_close(&tarFile);
    std::remove(tarFilePath.c_str());

    return true;
}

bool BackupRestore::ReplaceUserFiles()
{
    /* replace existing files that have respective backup files existing */
    const auto backupOSPath = purefs::dir::getBackupOSPath();
    if (std::filesystem::is_directory(backupOSPath) && std::filesystem::is_empty(backupOSPath)) {
        LOG_INFO("ReplaceUserFiles: dir emtpy, nothing to restore, quitting...");
        return false;
    }
    std::string userFilePath = purefs::dir::getUserDiskPath();
    userFilePath += "/";

    std::string backupFilePath = purefs::dir::getBackupOSPath();
    backupFilePath += "/";

    for (auto &direntry : std::filesystem::directory_iterator(backupOSPath.c_str())) {
        if ((direntry.path().compare(".") != 0) && (direntry.path().compare("..") != 0) &&
            (direntry.path().compare("...") != 0)) {
            LOG_INFO("ReplaceUserFiles: restoring backup file %s...", direntry.path().c_str());

            if (std::filesystem::exists((userFilePath + direntry.path().string()))) {
                if (std::filesystem::remove((userFilePath + direntry.path().string()))) {
                    try {
                        std::filesystem::rename(backupFilePath + direntry.path().string(),
                                                userFilePath + direntry.path().string());
                        LOG_INFO("ReplaceUserFiles: restoring backup file %s succeeded", direntry.path().c_str());
                    }
                    catch (const std::exception &e) {
                        LOG_ERROR("ReplaceUserFiles: restoring backup file %s failed on error %s",
                                  direntry.path().c_str(),
                                  e.what());
                    }
                }
                else {
                    LOG_ERROR("ReplaceUserFiles: restoring backup file %s", direntry.path().c_str());
                }
            }
            else {
                try {
                    std::filesystem::rename(backupFilePath + direntry.path().string(),
                                            userFilePath + direntry.path().string());
                    LOG_INFO("ReplaceUserFiles: restoring backup file %s succeeded", direntry.path().c_str());
                }
                catch (const std::filesystem::filesystem_error &e) {
                    LOG_ERROR("ReplaceUserFiles: restoring backup file %s failed on error %s",
                              direntry.path().c_str(),
                              e.what());
                }
            }
        }
    }

    return true;
}
