#include "BackupRestore.hpp"
#include "vfs.hpp"
#include "microtar/src/microtar.hpp"
#include "api/DBServiceAPI.hpp"
#include "service-db/includes/DBServiceName.hpp"
#include "SystemManager/SystemManager.hpp"

void BackupRestore::BackupUserFiles(sys::Service *ownerService)
{

    /* TODO: check nullptr for ownerService */

    if (BackupRestore::CleanBackupDir() == false) {
        return;
    }

    if (BackupRestore::CreateBackupDir() == false) {
        return;
    }

    /* store databse files into backup dir */
    std::string backupPathDB = PATH_BACKUPS;
    backupPathDB += "/";

    if (DBServiceAPI::DBBackup(ownerService, backupPathDB) == false) {
        LOG_ERROR("BackupUserFiles: backup database failed.");

        BackupRestore::CleanBackupDir();

        return;
    }

    std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(purefs::dir::os_backups.c_str(), "", true);

    if (dirlist.size() <= 2) {
        /* vfs.listdir also lists two directories ".." and "..." by default */
        LOG_INFO("BackupUserFiles: backup dir %s is empty, nothing to backup.", purefs::dir::os_backups.c_str());
        return;
    }

    LOG_INFO(
        "BackupUserFiles: backup dir %s listed with %d files.", purefs::dir::os_backups.c_str(), dirlist.size() - 2);

    /* archive files inside backup dir into .tar file */
    std::string backupFileTAR = purefs::dir::os_backups;
    backupFileTAR += "/";
    backupFileTAR += "backup.tar";

    mtar_t backupTar;

    LOG_INFO("BackupUserFiles: opening backup tar file %s...", backupFileTAR.c_str());

    int ret = mtar_open(&backupTar, backupFileTAR.c_str(), "w");

    if (ret != MTAR_ESUCCESS) {
        LOG_ERROR("BackupUserFiles: opening backup tar file %s failed with error: %s.",
                  backupFileTAR.c_str(),
                  mtar_strerror(ret));

        BackupRestore::CleanBackupDir();

        return;
    }

    std::unique_ptr<unsigned char[]> buffer(new unsigned char[purefs::buffer::tar_buf]);

    for (auto &direntry : dirlist) {
        if ((direntry.fileName.compare(".") != 0) && (direntry.fileName.compare("..") != 0) &&
            (direntry.fileName.compare("...") != 0)) {
            LOG_INFO("BackupUserFiles: archiving file %s...", (backupPathDB + direntry.fileName).c_str());

            vfs::FILE *file = vfs.fopen((backupPathDB + direntry.fileName).c_str(), "r");

            if (file == nullptr) {
                LOG_ERROR("BackupUserFiles: archiving file %s failed, cannot open file.", direntry.fileName.c_str());
                continue;
            }

            LOG_INFO("BackupUserFiles: writing tar header for %s...", direntry.fileName.c_str());

            if (mtar_write_file_header(&backupTar, direntry.fileName.c_str(), (unsigned)(direntry.fileSize)) !=
                MTAR_ESUCCESS) {
                LOG_ERROR("BackupUserFiles: writing tar header for %s failed.", direntry.fileName.c_str());
            }
            else {
                uint32_t loopcount = (file->ulFileSize / purefs::buffer::tar_buf) + 1u;
                uint32_t readsize  = 0u;

                for (uint32_t i = 0u; i < loopcount; i++) {
                    if (i + 1u == loopcount) {
                        readsize = file->ulFileSize % purefs::buffer::tar_buf;
                    }
                    else {
                        readsize = purefs::buffer::tar_buf;
                    }

                    LOG_INFO("BackupUserFiles: reading file %s...", direntry.fileName.c_str());

                    if (vfs.fread(buffer.get(), 1, readsize, file) == readsize) {
                        LOG_INFO("BackupUserFiles: writing tar file for %s...", direntry.fileName.c_str());

                        if (mtar_write_data(&backupTar, buffer.get(), readsize) != MTAR_ESUCCESS) {
                            LOG_ERROR("BackupUserFiles: writing tar file for %s failed.", direntry.fileName.c_str());
                        }
                    }
                    else {
                        LOG_INFO("BackupUserFiles: reading file %s failed.", direntry.fileName.c_str());
                    }
                }
            }

            LOG_INFO("BackupUserFiles: closing file %s...", (backupPathDB + direntry.fileName).c_str());

            if (vfs.fclose(file) == 0) {
                LOG_INFO("BackupUserFiles: deleting file %s...", (backupPathDB + direntry.fileName).c_str());

                if (vfs.remove((backupPathDB + direntry.fileName).c_str()) != 0) {
                    LOG_ERROR("BackupUserFiles: deleting file %s failed.", (backupPathDB + direntry.fileName).c_str());
                }
            }
            else {
                LOG_ERROR("BackupUserFiles: closing file %s failed.", (backupPathDB + direntry.fileName).c_str());
            }
        }
    }

    if (mtar_finalize(&backupTar) == MTAR_ESUCCESS) {
        LOG_INFO("BackupUserFiles: backup mtar_finalize succeeded.");

        if (mtar_close(&backupTar) == MTAR_ESUCCESS) {
            LOG_INFO("BackupUserFiles: backup mtar_close succeeded.");
        }
        else {
            LOG_ERROR("BackupUserFiles: backup mtar_close failed.");
            /* TODO: delete failed file from disk, report error */
        }
    }
    else {
        LOG_ERROR("BackupUserFiles: backup mtar_fnalize failed.");
        /* TODO: close failed file and delete from disk, report error */
    }
}

void BackupRestore::RestoreUserFiles(sys::Service *ownerService)
{
    std::string tarFilePath = purefs::dir::os_backups;
    tarFilePath += "/";
    tarFilePath += "backup.tar";

    mtar_t tarFile;
    mtar_header_t tarHeader;

    LOG_INFO("RestoreUserFiles: opening tar file %s...", tarFilePath.c_str());

    int ret = mtar_open(&tarFile, tarFilePath.c_str(), "r");

    if (ret != MTAR_ESUCCESS) {
        LOG_ERROR(
            "RestoreUserFiles: opening tar file %s failed with error: %s.", tarFilePath.c_str(), mtar_strerror(ret));

        return;
    }

    std::unique_ptr<unsigned char[]> buffer(new unsigned char[purefs::buffer::tar_buf]);

    do {
        ret = mtar_read_header(&tarFile, &tarHeader);
        LOG_INFO("RestoreUserFiles: reading tar header status %s.", mtar_strerror(ret));
        LOG_INFO("RestoreUserFiles: reading tar header type %d.", tarHeader.type);
        LOG_INFO("RestoreUserFiles: reading tar header name %s.", tarHeader.name);
        LOG_INFO("RestoreUserFiles: reading tar header size %d.", tarHeader.size);

        LOG_INFO("RestoreUserFiles: reading tar header %s...", tarHeader.name);

        if ((tarHeader.type == MTAR_TREG) && (ret == MTAR_ESUCCESS)) {
            std::string restoreFilePath = purefs::dir::os_backups;
            restoreFilePath += "/";
            restoreFilePath += tarHeader.name;
            vfs::FILE *file = vfs.fopen(restoreFilePath.c_str(), "w");

            if (file == nullptr) {
                LOG_ERROR("RestoreUserFiles: extracting file %s failed, cannot open file.", tarHeader.name);
                mtar_next(&tarFile);
                continue;
            }

            uint32_t loopcount = (tarHeader.size / purefs::buffer::tar_buf) + 1u;
            uint32_t readsize  = 0u;

            for (uint32_t i = 0u; i < loopcount; i++) {
                LOG_INFO("RestoreUserFiles: extracting file %s...", tarHeader.name);

                if (i + 1u == loopcount) {
                    readsize = tarHeader.size % purefs::buffer::tar_buf;
                }
                else {
                    readsize = purefs::buffer::tar_buf;
                }

                mtar_read_data(&tarFile, buffer.get(), readsize);
                vfs.fwrite(buffer.get(), readsize, 1, file);
            }

            vfs.fclose(file);
            LOG_INFO("RestoreUserFiles: extracting file %s succeeded.", tarHeader.name);
        }
        else {
            /* ignore all non-file contents? */
            LOG_INFO("RestoreUserFiles: found header %d, skipping.", tarHeader.type);
        }

        ret = mtar_next(&tarFile);
        LOG_INFO("RestoreUserFiles: reading tar next status %s.", mtar_strerror(ret));
    } while (ret == MTAR_ESUCCESS);

    LOG_INFO("RestoreUserFiles: cleaning directory from tar file...");
    mtar_close(&tarFile);
    vfs.remove(tarFilePath.c_str());

    /* close user files to be restored */
    LOG_INFO("RestoreUserFiles: closing ServiceDB...");
    std::string dbServiceName = service::name::db;
    sys::SystemManager::DestroyService(dbServiceName, ownerService);

    // replace existing files that have respective backup files existing
    std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(purefs::dir::os_backups.c_str());

    if (dirlist.size() <= 2) {
        LOG_INFO("RestoreUserFiles: dir emtpy, nothing to restore");
        return;
    }

    LOG_INFO("RestoreUserFiles: dir listed with %d files", dirlist.size() - 2);

    std::string userFilePath = purefs::dir::user_disk;
    userFilePath += "/";

    std::string backupFilePath = purefs::dir::os_backups;
    backupFilePath += "/";

    for (auto &direntry : dirlist) {

        if ((direntry.fileName.compare(".") != 0) && (direntry.fileName.compare("..") != 0) &&
            (direntry.fileName.compare("...") != 0)) {
            LOG_INFO("RestoreUserFiles: restoring backup file %s...", direntry.fileName.c_str());

            if (vfs.fileExists((userFilePath + direntry.fileName).c_str())) {
                if (vfs.remove((userFilePath + direntry.fileName).c_str()) == 0) {
                    if (vfs.rename((backupFilePath + direntry.fileName).c_str(),
                                   (userFilePath + direntry.fileName).c_str()) == 0) {
                        LOG_INFO("RestoreFromBackup: restoring backup file %s succeeded", direntry.fileName.c_str());
                    }
                    else {
                        LOG_ERROR("RestoreFromBackup: restoring backup file %s failed on error %s",
                                  direntry.fileName.c_str(),
                                  vfs.lastErrnoToStr().c_str());
                    }
                }
                else {
                    LOG_ERROR("RestoreFromBackup: restoring backup file %s failed on error %s",
                              direntry.fileName.c_str(),
                              vfs.lastErrnoToStr().c_str());
                }
            }
            else {
                if (vfs.rename((backupFilePath + direntry.fileName).c_str(),
                               (userFilePath + direntry.fileName).c_str()) == 0) {
                    LOG_INFO("RestoreFromBackup: restoring backup file %s succeeded", direntry.fileName.c_str());
                }
                else {
                    LOG_ERROR("RestoreFromBackup: restoring backup file %s failed on error %s",
                              direntry.fileName.c_str(),
                              vfs.lastErrnoToStr().c_str());
                }
            }
        }
    }

    LOG_INFO("RestoreUserFiles: restoring completed, rebooting...");
    sys::SystemManager::Reboot(ownerService);
}

bool BackupRestore::CleanBackupDir()
{
    /* prepare directories */
    if (vfs.isDir(purefs::dir::os_backups.c_str())) {
        LOG_INFO("BackupUserFiles: cleaning backup directory %s.", purefs::dir::os_backups.c_str());

        if (vfs.deltree(purefs::dir::os_backups.c_str()) != 0) {
            LOG_ERROR("BackupUserFiles: cleaning backup directory %s failed with error: %s.",
                      purefs::dir::os_backups.c_str(),
                      vfs.lastErrnoToStr().c_str());
            return false;
        }
    }

    return true;
}

bool BackupRestore::CreateBackupDir()
{
    if (vfs.mkdir(purefs::dir::os_backups.c_str())) {
        LOG_ERROR("BackupUserFiles: creating backup directory %s failed with error: %s.",
                  purefs::dir::os_backups.c_str(),
                  vfs.lastErrnoToStr().c_str());
        return false;
    }

    return true;
}

bool BackupRestore::OpenTarFile()
{
    /*
    mtar_t backupTar;

    LOG_INFO("BackupUserFiles: opening backup tar file %s...", backupFileTAR.c_str());

    int ret = mtar_open(&backupTar, backupFileTAR.c_str(), "w");

    if (ret != MTAR_ESUCCESS)
    {
        LOG_ERROR("BackupUserFiles: opening backup tar file %s failed with error: %s.", backupFileTAR.c_str(),
    mtar_strerror(ret));

        BackupRestore::CleanBackupDir();

        return;
    }
    */
    return false;
}
