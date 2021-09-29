// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-desktop/BackupRestore.hpp>
#include <endpoints/JsonKeyNames.hpp>

#include <SystemManager/SystemManagerCommon.hpp>
#include <log/log.hpp>
#include <microtar.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/DBServiceName.hpp>
#include <Utils.hpp>

#include <cassert>
#include <filesystem>
#include <purefs/filesystem_paths.hpp>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/statvfs.h>
#include <gsl/gsl>
#include <json11.hpp>

namespace sys
{
    class Service;
} // namespace sys

namespace bkp
{
    inline constexpr auto backupInfo = "backup.json";
};

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
    auto end          = gsl::finally([fromFp, toFp] {
        std::fclose(fromFp);
        std::fclose(toFp);
    });
    if (fromFp == nullptr) {
        LOG_ERROR("can't open file for reading");
        return false;
    }
    if (toFp == nullptr) {
        LOG_ERROR("can't open file for writing");
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

BackupRestore::CompletionCode BackupRestore::BackupUserFiles(sys::Service *ownerService, std::filesystem::path &path)
{
    assert(ownerService != nullptr);
    LOG_INFO("Backup started...");

    if (BackupRestore::RemoveBackupDir(path) == false) {
        return CompletionCode::FSError;
    }

    if (BackupRestore::CreateBackupDir(path) == false) {
        return CompletionCode::FSError;
    }

    LOG_DEBUG("Database backup started...");
    if (DBServiceAPI::DBBackup(ownerService, path) == false) {
        LOG_ERROR("Database backup failed, quitting...");
        BackupRestore::RemoveBackupDir(path);
        return CompletionCode::DBError;
    }

    if (WriteBackupInfo(ownerService, path) == false) {
        LOG_ERROR("Failed to save backup info");
        BackupRestore::RemoveBackupDir(path);
        return CompletionCode::CopyError;
    }

    LOG_DEBUG("Packing files");
    if (BackupRestore::PackUserFiles(path) == false) {
        LOG_ERROR("Failed pack backup");
        BackupRestore::RemoveBackupDir(path);
        return CompletionCode::PackError;
    }

    return CompletionCode::Success;
}

bool BackupRestore::WriteBackupInfo(sys::Service *ownerService, const std::filesystem::path &path)
{
    LOG_INFO("Writing backup info");

    if (!std::filesystem::is_directory(path)) {
        LOG_ERROR("%s is not a directory", path.c_str());
        return false;
    }

    const auto version_json = purefs::dir::getCurrentOSPath() / purefs::file::version_json;

    auto success = copyFile(version_json, path / bkp::backupInfo);

    if (!success) {
        LOG_ERROR("Failed to copy %s -> %s", (version_json).c_str(), (path / bkp::backupInfo).c_str());

        return false;
    }

    LOG_DEBUG("%s copied to %s", (version_json).c_str(), (path / bkp::backupInfo).c_str());
    return true;
}

BackupRestore::CompletionCode BackupRestore::RestoreUserFiles(sys::Service *ownerService,
                                                              const std::filesystem::path &path)
{
    assert(ownerService != nullptr);

    LOG_INFO("Restore started");

    if (BackupRestore::UnpackBackupFile(path) == false) {
        LOG_ERROR("Can't unpack user files");
        return CompletionCode::UnpackError;
    }

    if (BackupRestore::CanRestoreFromBackup(TempPathForBackupFile(path)) == false) {
        LOG_ERROR("Can't restore user files");
        return CompletionCode::FSError;
    }

    if (sys::SystemManagerCommon::Restore(ownerService) == false) {
        LOG_ERROR("Can't enter system restore state");
        return CompletionCode::OtherError;
    }

    LOG_INFO("Entered restore state");

    if (BackupRestore::ReplaceUserFiles(path) == false) {
        LOG_ERROR("Can't restore user files");
        return CompletionCode::CopyError;
    }

    return CompletionCode::Success;
}

bool BackupRestore::RemoveBackupDir(const std::filesystem::path &path)
{
    /* prepare directories */
    if (std::filesystem::is_directory(path)) {
        LOG_INFO("Removing backup directory %s...", path.c_str());

        std::error_code errorCode;
        std::filesystem::remove_all(path, errorCode);

        if (errorCode) {
            LOG_ERROR("Removing backup directory %s failed, error: %s.", path.c_str(), errorCode.message().c_str());
            return false;
        }
    }

    return true;
}

bool BackupRestore::CreateBackupDir(const std::filesystem::path &path)
{
    LOG_INFO("Creating backup directory %s...", path.c_str());
    std::error_code errorCode;

    if (!std::filesystem::is_directory(purefs::dir::getBackupOSPath())) {
        if (!std::filesystem::create_directory(purefs::dir::getBackupOSPath(), errorCode)) {
            LOG_ERROR("Failed to create directory: %s, error. \"%s\"",
                      purefs::dir::getBackupOSPath().c_str(),
                      errorCode.message().c_str());
            return false;
        }
    }

    if (!std::filesystem::is_directory(purefs::dir::getTemporaryPath())) {
        if (!std::filesystem::create_directory(purefs::dir::getTemporaryPath(), errorCode)) {
            LOG_ERROR("Creating backup directory %s failed. \"%s\"",
                      purefs::dir::getTemporaryPath().c_str(),
                      errorCode.message().c_str());
            return false;
        }
    }

    if (!std::filesystem::is_directory(path)) {
        if (!std::filesystem::create_directory(path, errorCode)) {
            LOG_ERROR("Failed to create directory: %s, error. \"%s\"", path.c_str(), errorCode.message().c_str());
            return false;
        }
    }

    return true;
}

bool BackupRestore::PackUserFiles(const std::filesystem::path &path)
{
    if (std::filesystem::is_empty(path)) {
        LOG_ERROR("Backup dir is empty, nothing to backup, quitting...");
        BackupRestore::RemoveBackupDir(path);
        return false;
    }

    std::filesystem::path tarFilePath = (purefs::dir::getBackupOSPath() / path.filename());
    mtar_t tarFile;

    LOG_INFO("Opening tar %s file...", tarFilePath.c_str());

    int ret = mtar_open(&tarFile, tarFilePath.c_str(), "w");

    if (ret != MTAR_ESUCCESS) {
        LOG_ERROR("Opening tar file failed, quitting...");
        BackupRestore::RemoveBackupDir(path);
        return false;
    }

    auto buffer = std::make_unique<unsigned char[]>(purefs::buffer::tar_buf);
    std::error_code errorCode;

    for (auto &direntry : std::filesystem::directory_iterator(path)) {
        if (!isValidDirentry(direntry)) {
            continue;
        }

        LOG_INFO("Archiving file ...");
        auto *file = std::fopen(direntry.path().string().c_str(), "r");

        if (file == nullptr) {
            LOG_ERROR("Archiving file failed, cannot open file, quitting...");
            mtar_close(&tarFile);
            BackupRestore::RemoveBackupDir(path);
            return false;
        }

        LOG_DEBUG("Writting tar header ...");

        if (mtar_write_file_header(&tarFile,
                                   direntry.path().filename().c_str(),
                                   static_cast<unsigned>(std::filesystem::file_size(direntry))) != MTAR_ESUCCESS) {
            LOG_ERROR("Writing tar header failed");
            std::fclose(file);
            mtar_close(&tarFile);
            BackupRestore::RemoveBackupDir(path);
            return false;
        }

        uintmax_t filesize = std::filesystem::file_size(direntry.path(), errorCode);
        if (errorCode) {
            LOG_ERROR("Failed to get size for file");
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

            LOG_DEBUG("Reading file ...");

            if (std::fread(buffer.get(), 1, readsize, file) != readsize) {
                LOG_ERROR("Reading file failed, quitting...");
                std::fclose(file);
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir(path);
                return false;
            }

            LOG_DEBUG("Writting into backup...");
            if (mtar_write_data(&tarFile, buffer.get(), readsize) != MTAR_ESUCCESS) {
                LOG_ERROR("Writting into backup failed, quitting...");
                std::fclose(file);
                mtar_close(&tarFile);
                BackupRestore::RemoveBackupDir(path);
                return false;
            }
        }

        LOG_INFO("Closing file...");
        if (std::fclose(file) != 0) {
            LOG_ERROR("Closing file failed, quitting...");
            mtar_close(&tarFile);
            BackupRestore::RemoveBackupDir(path);
            return false;
        }

        LOG_INFO("Deleting file ...");

        if (std::remove(direntry.path().c_str()) != 0) {
            LOG_ERROR("Deleting file failed, quitting...");
            mtar_close(&tarFile);
            BackupRestore::RemoveBackupDir(path);
            return false;
        }
    }

    LOG_INFO("Finalizing tar file...");
    if (mtar_finalize(&tarFile) != MTAR_ESUCCESS) {
        LOG_ERROR("Finalizing tar file failed, quitting....");
        mtar_close(&tarFile);
        BackupRestore::RemoveBackupDir(path);
        return false;
    }

    LOG_INFO("Closing tar file...");
    if (mtar_close(&tarFile) != MTAR_ESUCCESS) {
        LOG_ERROR("Closing tar file failed, quitting...");
        BackupRestore::RemoveBackupDir(path);
        return false;
    }

    return true;
}

auto BackupRestore::TempPathForBackupFile(const std::filesystem::path &tarFilePath) -> std::filesystem::path const
{
    std::filesystem::path extractFolder;

    if (tarFilePath.has_extension()) {
        extractFolder = tarFilePath.stem();
    }
    else {
        extractFolder = tarFilePath.filename();
    }

    return purefs::dir::getTemporaryPath() / extractFolder;
}

bool BackupRestore::UnpackBackupFile(const std::filesystem::path &tarFilePath)
{
    mtar_t tarFile;
    mtar_header_t tarHeader;
    std::error_code errorCode;

    auto extractDestination = TempPathForBackupFile(tarFilePath);

    LOG_INFO("Creating temporary directory");
    if (!std::filesystem::is_directory(extractDestination, errorCode)) {
        std::filesystem::create_directory(extractDestination, errorCode);

        if (errorCode) {
            LOG_ERROR("Can't create temporary directory");
            return false;
        }
    }

    LOG_INFO("Opening tar file ...");

    int ret = mtar_open(&tarFile, tarFilePath.c_str(), "r");

    if (ret != MTAR_ESUCCESS) {
        LOG_ERROR("Opening tar file failed, quitting...");
        return false;
    }

    auto buffer = std::make_unique<unsigned char[]>(purefs::buffer::tar_buf);

    do {
        ret = mtar_read_header(&tarFile, &tarHeader);
        LOG_DEBUG("Reading tar header name...");

        if ((tarHeader.type == MTAR_TREG) && (ret == MTAR_ESUCCESS)) {
            LOG_DEBUG("Extracting tar file ...");

            std::filesystem::path extractedFile = extractDestination / tarHeader.name;
            auto *file                          = std::fopen(extractedFile.c_str(), "w");

            if (file == nullptr) {
                LOG_ERROR("Can't open file for writing");
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
                    LOG_ERROR("Extracting file failed, quitting...");
                    mtar_close(&tarFile);
                    std::fclose(file);
                    std::remove(extractedFile.c_str());
                    return false;
                }

                if (std::fwrite(buffer.get(), 1, readsize, file) != readsize) {
                    LOG_ERROR("Writting file failed, quitting...");
                    mtar_close(&tarFile);
                    std::fclose(file);
                    std::remove(extractedFile.c_str());
                    return false;
                }
            }

            LOG_INFO("Extracting file succeeded");
            std::fclose(file);
        }
        else {
            LOG_DEBUG("Found header %d, skipping", tarHeader.type);
        }

        ret = mtar_next(&tarFile);
        LOG_DEBUG("Reading tar next status %s", mtar_strerror(ret));
    } while (ret == MTAR_ESUCCESS);

    LOG_DEBUG("Cleanup");
    mtar_close(&tarFile);
    std::remove(tarFilePath.c_str());

    if (errorCode) {
        LOG_WARN("Can't cleanup temporary dir");
    }

    return true;
}

std::string BackupRestore::ReadFileAsString(const std::filesystem::path &fileToRead)
{
    const auto file = std::ifstream(fileToRead);

    if (!file.is_open()) {
        LOG_ERROR("Can't open %s for compatibility check", fileToRead.string().c_str());
        return {};
    }

    std::ostringstream fileContents;
    fileContents << file.rdbuf();

    return fileContents.str();
}

using namespace sdesktop::endpoints;

bool BackupRestore::CanRestoreFromBackup(const std::filesystem::path &extractedBackup)
{
    const auto backup_info  = ReadFileAsString(extractedBackup / bkp::backupInfo);
    const auto version_json = ReadFileAsString(purefs::dir::getCurrentOSPath() / purefs::file::version_json);

    if (backup_info.empty()) {
        LOG_ERROR("Can't read backup.info for compatibility check");
        return false;
    }

    if (version_json.empty()) {
        LOG_ERROR("Can't read version.json for compatibility check");
        return false;
    }

    std::string err;
    json11::Json backup_info_Root = json11::Json::parse(backup_info, err);
    if (backup_info_Root.is_null()) {
        LOG_ERROR("Can't parse backup.info: %s", err.c_str());
        return false;
    }

    json11::Json version_json_Root = json11::Json::parse(version_json, err);
    if (version_json_Root.is_null()) {
        LOG_ERROR("Can't parse version.json: %s", err.c_str());
        return false;
    }

    auto const currentVersion = version_json_Root[json::version];
    auto const backupVersion  = backup_info_Root[json::version];

    if (currentVersion[json::major] < backupVersion[json::major] ||
        currentVersion[json::minor] < backupVersion[json::minor]) {
        LOG_ERROR("Current OS version %s older than backup version %s",
                  (currentVersion[json::versionString].string_value()).c_str(),
                  (backupVersion[json::versionString].string_value()).c_str());
        return false;
    }

    return true;
}

bool BackupRestore::ReplaceUserFiles(const std::filesystem::path &path)
{
    /* replace existing files that have respective backup files existing */
    const auto tempDir = purefs::dir::getTemporaryPath() / path.stem();

    if (std::filesystem::is_directory(tempDir) && std::filesystem::is_empty(tempDir)) {
        LOG_INFO("Dir empty, nothing to restore, quitting...");
        return false;
    }

    const std::filesystem::path userDir   = purefs::dir::getUserDiskPath();
    const std::filesystem::path backupDir = purefs::dir::getBackupOSPath();
    std::error_code errorCode;

    for (auto &direntry : std::filesystem::directory_iterator(tempDir, errorCode)) {
        if (errorCode) {
            LOG_INFO("Can't list contents of temp dir");
            return false;
        }

        if (!isValidDirentry(direntry)) {
            continue;
        }

        // dont restore the information file
        if (direntry.path().filename() == bkp::backupInfo) {
            continue;
        }

        LOG_INFO("Restoring backup file ...");

        if (std::filesystem::remove(userDir / direntry.path().filename(), errorCode)) {
            std::filesystem::rename(
                tempDir / direntry.path().filename(), userDir / direntry.path().filename(), errorCode);
            if (errorCode) {
                LOG_ERROR("Can't move file. Restore failed");
                return false;
            }
        }
        else {
            LOG_WARN("Can't remove file");
            // we should continue, there can be new files in the backup
        }
    }

    return true;
}

json11::Json BackupRestore::GetBackupFiles()
{
    auto dirEntryVector = std::vector<std::string>();
    std::error_code errorCode;
    for (const auto &p : std::filesystem::directory_iterator(purefs::dir::getBackupOSPath(), errorCode)) {
        if (errorCode) {
            LOG_ERROR("Can't get directory %s contents", purefs::dir::getBackupOSPath().c_str());
            return json11::Json();
        }
        if (!p.is_directory()) {
            LOG_DEBUG("Possible restore file");
            dirEntryVector.push_back(p.path().filename());
        }
    }

    return dirEntryVector;
}
