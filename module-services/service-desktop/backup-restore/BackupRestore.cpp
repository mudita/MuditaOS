#include "BackupRestore.hpp"
#include "vfs.hpp"
#include "microtar/src/microtar.hpp"
#include "api/DBServiceAPI.hpp"
#include "service-db/includes/DBServiceName.hpp"
#include "SystemManager/SystemManager.hpp"

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
    if (vfs.isDir(purefs::dir::os_backup.c_str())) {
        LOG_INFO("RemoveBackupDir: removing backup directory %s...", purefs::dir::os_backup.c_str());

        if (vfs.deltree(purefs::dir::os_backup.c_str()) != 0) {
            LOG_ERROR("RemoveBackupDir: removing backup directory %s failed, error: %s.",
                      purefs::dir::os_backup.c_str(),
                      vfs.lastErrnoToStr().c_str());
            return false;
        }
    }

    return true;
}

bool BackupRestore::CreateBackupDir()
{
    LOG_INFO("CreateBackupDir: creating backup directory %s...", purefs::dir::os_backup.c_str());

    if (vfs.isDir(purefs::dir::os_backup.c_str()) == false) {
        if (vfs.mkdir(purefs::dir::os_backup.c_str()) != 0) {
            LOG_ERROR("CreateBackupDir: creating backup directory %s failed, error: %s.",
                      purefs::dir::os_backup.c_str(),
                      vfs.lastErrnoToStr().c_str());
            return false;
        }
    }

    return true;
}

bool BackupRestore::PackUserFiles()
{
    std::string backupPathDB = PATH_BACKUP;
    backupPathDB += "/";

    std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(purefs::dir::os_backup.c_str(), "", true);

    if (dirlist.size() <= empty_dirlist_size) {
        /* vfs.listdir also lists two extra directories ".." and "..." by default, they shall be ignored */
        LOG_ERROR("PackUserFiles: backup dir %s is empty, nothing to backup, quitting...",
                  purefs::dir::os_backup.c_str());
        BackupRestore::RemoveBackupDir();
        return false;
    }

    LOG_INFO("PackUserFiles: backup dir %s listed with %lu files.",
             purefs::dir::os_backup.c_str(),
             dirlist.size() - empty_dirlist_size);

    std::string tarFilePath = purefs::dir::os_backup;
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

    for (auto &direntry : dirlist) {
        if ((direntry.fileName.compare(".") != 0) && (direntry.fileName.compare("..") != 0) &&
            (direntry.fileName.compare("...") != 0)) {

            LOG_INFO("PackUserFiles: archiving file %s...", (backupPathDB + direntry.fileName).c_str());
            vfs::FILE *file = vfs.fopen((backupPathDB + direntry.fileName).c_str(), "r");

            if (file == nullptr) {
                LOG_ERROR("PackUserFiles: archiving file %s failed, cannot open file, quitting...",
                          direntry.fileName.c_str());
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir();
                return false;
            }

            LOG_INFO("PackUserFiles: writting tar header for %s...", direntry.fileName.c_str());

            if (mtar_write_file_header(&tarFile, direntry.fileName.c_str(), static_cast<unsigned>(direntry.fileSize)) !=
                MTAR_ESUCCESS) {
                LOG_ERROR("PackUserFiles: writing tar header for %s failed", direntry.fileName.c_str());
                vfs.fclose(file);
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir();
                return false;
            }

            uint32_t loopcount = (vfs.filelength(file) / purefs::buffer::tar_buf) + 1u;
            uint32_t readsize  = 0u;

            for (uint32_t i = 0u; i < loopcount; i++) {
                if (i + 1u == loopcount) {
                    readsize = vfs.filelength(file) % purefs::buffer::tar_buf;
                }
                else {
                    readsize = purefs::buffer::tar_buf;
                }

                LOG_INFO("PackUserFiles: reading file %s...", direntry.fileName.c_str());

                if (vfs.fread(buffer.get(), 1, readsize, file) != readsize) {
                    LOG_ERROR("PackUserFiles: reading file %s failed, quitting...", direntry.fileName.c_str());
                    vfs.fclose(file);
                    mtar_close(&tarFile);
                    BackupRestore::RemoveBackupDir();
                    return false;
                }

                LOG_INFO("PackUserFiles: writting %s into backup...", direntry.fileName.c_str());
                if (mtar_write_data(&tarFile, buffer.get(), readsize) != MTAR_ESUCCESS) {
                    LOG_ERROR("PackUserFiles: writting %s into backup failed, quitting...", direntry.fileName.c_str());
                    vfs.fclose(file);
                    mtar_close(&tarFile);
                    BackupRestore::RemoveBackupDir();
                    return false;
                }
            }

            LOG_INFO("PackUserFiles: closing file %s...", (backupPathDB + direntry.fileName).c_str());
            if (vfs.fclose(file) != 0) {
                LOG_ERROR("PackUserFiles: closing file %s failed, quitting...",
                          (backupPathDB + direntry.fileName).c_str());
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir();
                return false;
            }

            LOG_INFO("PackUserFiles: deleting file %s...", (backupPathDB + direntry.fileName).c_str());
            if (vfs.remove((backupPathDB + direntry.fileName).c_str()) != 0) {
                LOG_ERROR("PackUserFiles: deleting file %s failed, quitting...",
                          (backupPathDB + direntry.fileName).c_str());
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
    std::string tarFilePath = purefs::dir::os_backup;
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

            std::string restoreFilePath = purefs::dir::os_backup;
            restoreFilePath += "/";
            restoreFilePath += tarHeader.name;
            vfs::FILE *file = vfs.fopen(restoreFilePath.c_str(), "w");

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
                    vfs.fclose(file);
                    vfs.remove(restoreFilePath.c_str());
                    return false;
                }

                if (vfs.fwrite(buffer.get(), 1, readsize, file) != readsize) {
                    LOG_ERROR("UnpackBackupFile: writting file %s failed, quitting...", restoreFilePath.c_str());
                    mtar_close(&tarFile);
                    vfs.fclose(file);
                    vfs.remove(restoreFilePath.c_str());
                    return false;
                }
            }

            LOG_INFO("UnpackBackupFile: extracting file %s succeeded", tarHeader.name);
            vfs.fclose(file);
        }
        else {
            LOG_INFO("UnpackBackupFile: found header %d, skipping", tarHeader.type);
        }

        ret = mtar_next(&tarFile);
        LOG_INFO("UnpackBackupFile: reading tar next status %s", mtar_strerror(ret));
    } while (ret == MTAR_ESUCCESS);

    LOG_INFO("UnpackBackupFile: cleaning directory from tar file...");
    mtar_close(&tarFile);
    vfs.remove(tarFilePath.c_str());

    return true;
}

bool BackupRestore::ReplaceUserFiles()
{
    /* replace existing files that have respective backup files existing */
    std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(purefs::dir::os_backup.c_str());

    if (dirlist.size() <= empty_dirlist_size) {
        LOG_INFO("ReplaceUserFiles: dir emtpy, nothing to restore, quitting...");
        return false;
    }

    LOG_INFO("ReplaceUserFiles: dir listed with %lu files", dirlist.size() - empty_dirlist_size);

    std::string userFilePath = purefs::dir::user_disk;
    userFilePath += "/";

    std::string backupFilePath = purefs::dir::os_backup;
    backupFilePath += "/";

    for (auto &direntry : dirlist) {
        if ((direntry.fileName.compare(".") != 0) && (direntry.fileName.compare("..") != 0) &&
            (direntry.fileName.compare("...") != 0)) {
            LOG_INFO("ReplaceUserFiles: restoring backup file %s...", direntry.fileName.c_str());

            if (vfs.fileExists((userFilePath + direntry.fileName).c_str())) {
                if (vfs.remove((userFilePath + direntry.fileName).c_str()) == 0) {
                    if (vfs.rename((backupFilePath + direntry.fileName).c_str(),
                                   (userFilePath + direntry.fileName).c_str()) == 0) {
                        LOG_INFO("ReplaceUserFiles: restoring backup file %s succeeded", direntry.fileName.c_str());
                    }
                    else {
                        LOG_ERROR("ReplaceUserFiles: restoring backup file %s failed on error %s",
                                  direntry.fileName.c_str(),
                                  vfs.lastErrnoToStr().c_str());
                    }
                }
                else {
                    LOG_ERROR("ReplaceUserFiles: restoring backup file %s failed on error %s",
                              direntry.fileName.c_str(),
                              vfs.lastErrnoToStr().c_str());
                }
            }
            else {
                if (vfs.rename((backupFilePath + direntry.fileName).c_str(),
                               (userFilePath + direntry.fileName).c_str()) == 0) {
                    LOG_INFO("ReplaceUserFiles: restoring backup file %s succeeded", direntry.fileName.c_str());
                }
                else {
                    LOG_ERROR("ReplaceUserFiles: restoring backup file %s failed on error %s",
                              direntry.fileName.c_str(),
                              vfs.lastErrnoToStr().c_str());
                }
            }
        }
    }

    return true;
}
