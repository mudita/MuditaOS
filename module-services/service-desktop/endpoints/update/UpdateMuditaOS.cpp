﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UpdateMuditaOS.hpp"
#include <service-desktop/ServiceDesktop.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <SystemManager/SystemManager.hpp>
#include <crc32.h>
#include <json11.hpp>
#include <log/log.hpp>
#include <application-desktop/Constants.hpp>
#include <service-db/service-db/Settings.hpp>
#include <purefs/filesystem_paths.hpp>
#include <filesystem>
#include <Utils.hpp>
#include <boot/bootconfig.hpp>
#include <boot/bootconstants.hpp>

#if defined(TARGET_RT1051)
#include <board/cross/eMMC/eMMC.hpp>
#include "bsp/watchdog/watchdog.hpp"
#endif

#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <memory>

FileInfo::FileInfo(mtar_header_t &h, unsigned long crc32) : fileSize(h.size), fileCRC32(crc32)
{
    if (h.name[0] == '.' && h.name[1] == '/') {
        // microtar relative paths, strip the leading ./away
        fileName = std::string(h.name).substr(2);
    }
    else {
        fileName = std::string(h.name);
    }
}

json11::Json FileInfo::to_json() const
{
    return json11::Json::object{
        {"name", fileName}, {"size", std::to_string(fileSize)}, {"crc32", std::to_string(fileCRC32)}};
}

UpdateMuditaOS::UpdateMuditaOS(ServiceDesktop *ownerService) : owner(ownerService)
{
    status = updateos::UpdateState::Initial;
    bootConfig.load();
}

updateos::UpdateError UpdateMuditaOS::setUpdateFile(const std::filesystem::path &updatesOSPath,
                                                    const fs::path &updateFileToUse)
{
    if (isUpdateToBeAborted()) {
        setUpdateAbortFlag(false);
        sys::SystemManager::Reboot(owner);
        return informError(updateos::UpdateError::UpdateAborted, "update aborted");
    }
    else {
        informUpdateWindow();
    }

    updateFile = purefs::dir::getUpdatesOSPath() / updateFileToUse;
    if (std::filesystem::exists(updateFile.c_str())) {
        versionInformation = UpdateMuditaOS::getVersionInfoFromFile(updateFile);
        if (mtar_open(&updateTar, updateFile.c_str(), "r") == MTAR_ESUCCESS) {
            totalBytes = std::filesystem::file_size(updateFile);
        }
        else {
            return informError(updateos::UpdateError::CantOpenUpdateFile,
                               "UpdateMuditaOS::setUpdateFile can't open TAR file %s",
                               updateFile.c_str());
        }
    }
    else {
        return informError(updateos::UpdateError::CantOpenUpdateFile,
                           "UpdateMuditaOS::setUpdateFile %s does not exist",
                           updateFile.c_str());
    }

    status = updateos::UpdateState::UpdateFileSet;

    informUpdateWindow();

    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::runUpdate()
{
    informDebug("Preparing temp dir");

    updateRunStatus.startTime   = static_cast<uint32_t>(std::time(nullptr));
    updateRunStatus.fromVersion = bootConfig.to_json()[boot::json::git_info];
    versionInformation          = UpdateMuditaOS::getVersionInfoFromFile(updateFile);

    auto currentOSVersion = bootConfig.os_version();
    auto updateOSVersion  = versionInformation[boot::json::os_version][boot::json::version_string].string_value();

    storeRunStatusInDB();

    updateos::UpdateError err =
        prepareTempDirForUpdate(purefs::dir::getTemporaryPath(), purefs::dir::getUpdatesOSPath());
    if (err != updateos::UpdateError::NoError) {
        if (err == updateos::UpdateError::UpdateAborted) {
            return informError(updateos::UpdateError::UpdateAborted, "update aborted");
        }
        else {
            return informError(err, "runUpdate can't prepare temp directory for update");
        }
    }

    informDebug("Unpacking update");
    err = unpackUpdate();
    if (err == updateos::UpdateError::NoError) {
        informUpdate(status, "Unpacked");
    }
    else if (err == updateos::UpdateError::UpdateAborted) {
        return informError(updateos::UpdateError::UpdateAborted, "update aborted");
    }
    else {
        return informError(err, "%s can't be unpacked", updateFile.c_str());
    }

    err = verifyChecksums();
    if (err == updateos::UpdateError::NoError) {
        informUpdate(status, "Verify checksums");
    }
    else if (err == updateos::UpdateError::UpdateAborted) {
        return informError(updateos::UpdateError::UpdateAborted, "update aborted");
    }
    else {
        return informError(err, "Checksum verification failed");
    }

    if ((err = verifyVersion()) == updateos::UpdateError::NoError) {
        informUpdate(status, "Verify version");
    }
    else {
        return informError(err, "Can't verify version");
    }

    // at this point we should set the system to update mode we are
    // writing directly to eMMC when updating the bootloader
    // then placing the new files in destination folders/files
    sys::SystemManager::Update(owner, updateOSVersion, currentOSVersion);

    if ((err = updateBootloader()) == updateos::UpdateError::NoError) {
        informUpdate(status, "Update bootloader");
    }
    else {
        return informError(err, "Failed to update the bootloader");
    }

    if ((err = prepareRoot()) == updateos::UpdateError::NoError) {
        informUpdate(status, "Ready for reset");
    }
    else {
        informError(err, "Can't prepare root dir for reset");
    }

    if ((err = cleanupAfterUpdate()) != updateos::UpdateError::NoError) {
        informError(err, "runUpdate cleanupAfterUpdate failed, resetting anyway");
    }

    updateRunStatus.endTime = static_cast<uint32_t>(std::time(nullptr));
    storeRunStatusInDB();

    // reboot always
    sys::SystemManager::Reboot(owner);

    sys::SystemManager::storeOsVersion(owner, updateOSVersion, updateOSVersion);

    return err;
}

updateos::UpdateError UpdateMuditaOS::unpackUpdate()
{
    status = updateos::UpdateState::ExtractingFiles;

    if (isUpdateToBeAborted()) {
        setUpdateAbortFlag(false);
        sys::SystemManager::Reboot(owner);
        return updateos::UpdateError::UpdateAborted;
    }
    else {
        informUpdateWindow();
    }

    mtar_header_t tarHeader;
    filesInUpdatePackage.clear();

    std::rewind(updateTar.stream);
    while ((mtar_read_header(&updateTar, &tarHeader)) != MTAR_ENULLRECORD) {
        if (isUpdateToBeAborted()) {
            setUpdateAbortFlag(false);
            sys::SystemManager::Reboot(owner);
            return updateos::UpdateError::UpdateAborted;
        }
        if (std::string(tarHeader.name) == "./") {
            mtar_next(&updateTar);
            continue;
        }
        unsigned long fileCRC32 = 0;
        if (tarHeader.type == MTAR_TDIR) {
            fs::path tmpPath = getUpdateTmpChild(tarHeader.name);
            if (!std::filesystem::create_directory(tmpPath.c_str())) {
                return informError(updateos::UpdateError::CantCreateExtractedFile,
                                   "unpackUpdate failed to create %s when extracting update tar",
                                   tmpPath.c_str());
            }
        }
        else {
            if (unpackFileToTemp(tarHeader, &fileCRC32) == false) {
                return informError(updateos::UpdateError::CantCreateExtractedFile,
                                   "unpackUpdate failed to extract update file %s",
                                   tarHeader.name);
            }
            filesInUpdatePackage.emplace_back(FileInfo(tarHeader, fileCRC32));
        }

        mtar_next(&updateTar);
    }

    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::verifyChecksums()
{
    LOG_DEBUG("Checksum Verify");
    status = updateos::UpdateState::ChecksumVerification;

    if (isUpdateToBeAborted()) {
        setUpdateAbortFlag(false);
        sys::SystemManager::Reboot(owner);
        return updateos::UpdateError::UpdateAborted;
    }
    else {
        informUpdateWindow();
    }

    fs::path checksumsFile = getUpdateTmpChild(updateos::file::checksums);
    std::ifstream fpChecksums(checksumsFile.string(), std::ios::binary);

    if (!fpChecksums.is_open()) {
        return informError(updateos::UpdateError::CantOpenChecksumsFile,
                           "verifyChecksums can't open checksums file %s",
                           checksumsFile.c_str());
    }

    while (!fpChecksums.eof()) {
        std::string line;
        std::getline(fpChecksums, line);

        std::string filePath;
        unsigned long fileCRC32;

        if (line[0] == ';') {
            continue;
        }

        getChecksumInfo(line, filePath, &fileCRC32);
        unsigned long computedCRC32 = getExtractedFileCRC32(filePath);
        if (computedCRC32 != fileCRC32) {
            fpChecksums.close();
            return informError(updateos::UpdateError::VerifyChecksumsFailure,
                               "verifyChecksums %s crc32 match FAIL %lX != %lX",
                               filePath.c_str(),
                               fileCRC32,
                               computedCRC32);
        }
    }
    fpChecksums.close();
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::verifyVersion()
{
    status = updateos::UpdateState::VersionVerificiation;

    if (isUpdateToBeAborted()) {
        setUpdateAbortFlag(false);
        sys::SystemManager::Reboot(owner);
        return updateos::UpdateError::UpdateAborted;
    }
    else {
        informUpdateWindow();
    }

    if (!std::filesystem::exists(getUpdateTmpChild(updateos::file::version).c_str())) {
        return informError(updateos::UpdateError::VerifyVersionFailure,
                           "verifyVersion %s does not exist",
                           getUpdateTmpChild(updateos::file::version).c_str());
    }

    std::ifstream version_file(getUpdateTmpChild(updateos::file::version).string(), std::ios::binary);
    std::string versionJsonString(std::istreambuf_iterator<char>(version_file), (std::istreambuf_iterator<char>()));
    std::string parserError;
    targetVersionInfo = json11::Json::parse(versionJsonString, parserError);
    if (!parserError.empty()) {
        return informError(
            updateos::UpdateError::VerifyVersionFailure, "verifyVersion parse json error: %s", parserError.c_str());
    }
    else {
        /* version comparison goes here */
        updateRunStatus.toVersion = targetVersionInfo[boot::json::git_info];
        const bool ret = bootConfig.version_compare(targetVersionInfo[boot::json::version_string].string_value(),
                                                    bootConfig.os_version());
        LOG_DEBUG("verifyVersion comparison result == %s", ret ? "true" : "false");
    }
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::updateBootloader()
{
    informDebug("updateBootloader");
    status = updateos::UpdateState::UpdatingBootloader;

    if (isUpdateToBeAborted()) {
        setUpdateAbortFlag(false);
        sys::SystemManager::Reboot(owner);
        return updateos::UpdateError::UpdateAborted;
    }
    else {
        informUpdateWindow();
    }

    if (targetVersionInfo[boot::json::bootloader][parserFSM::json::fileName].is_string()) {
        fs::path bootloaderFile =
            getUpdateTmpChild(targetVersionInfo[boot::json::bootloader][parserFSM::json::fileName].string_value());
        return writeBootloader(bootloaderFile);
    }
    return updateos::UpdateError::NoError;
}

unsigned long UpdateMuditaOS::getExtractedFileCRC32(const std::string &filePath)
{
    for (const auto &file : filesInUpdatePackage) {
        if (file.fileName == filePath) {
            return file.fileCRC32;
        }
    }
    return 0;
}

void UpdateMuditaOS::getChecksumInfo(const std::string &infoLine, std::string &filePath, unsigned long *fileCRC32Long)
{
    std::size_t lastSpacePos = infoLine.find_last_of(' ');
    if (lastSpacePos > 0) {
        filePath                       = infoLine.substr(0, lastSpacePos);
        const std::string fileCRC32Str = infoLine.substr(lastSpacePos + 1, boot::consts::crc_char_size);
        if (fileCRC32Long != nullptr) {
            *fileCRC32Long = strtoull(fileCRC32Str.c_str(), nullptr, boot::consts::crc_radix);
            informDebug("getChecksumInfo filePath: %s fileCRC32Str: %s fileCRC32Long: %lu fileCRC32Hex: %lX",
                        filePath.c_str(),
                        fileCRC32Str.c_str(),
                        *fileCRC32Long,
                        *fileCRC32Long);
        }
    }
}

updateos::UpdateError UpdateMuditaOS::prepareRoot()
{
    informDebug("prepareRoot()");
    // basic needed dirs

    status                    = updateos::UpdateState::VersionVerificiation;
    const auto previousOSPath = purefs::dir::getPreviousOSPath();
    const auto currentOSPath  = purefs::dir::getCurrentOSPath();
    const auto userDiskPath   = purefs::dir::getUserDiskPath();

    informDebug("prepareRoot mkdir: %s", previousOSPath.c_str());
    std::filesystem::create_directory(previousOSPath.c_str());
    informDebug("prepareRoot mkdir: %s", currentOSPath.c_str());
    std::filesystem::create_directory(currentOSPath.c_str());
    informDebug("prepareRoot mkdir: %s", userDiskPath.c_str());
    std::filesystem::create_directory(userDiskPath.c_str());

    // remove the previous OS version from partition
    informDebug("prepareRoot deltree: %s", previousOSPath.c_str());
    try {
        std::filesystem::remove_all(previousOSPath.c_str());
    }
    catch (const std::filesystem::filesystem_error &fsError) {
        return informError(updateos::UpdateError::CantDeletePreviousOS,
                           "prepareRoot remove_all on %s caused an error %s",
                           previousOSPath.c_str(),
                           fsError.what());
    }

    if (std::filesystem::is_directory(purefs::dir::getPreviousOSPath().c_str())) {
        return informError(
            updateos::UpdateError::CantDeletePreviousOS, "prepareRoot ff_deltree on %s", previousOSPath.c_str());
    }

    if (std::filesystem::is_directory(purefs::dir::getPreviousOSPath().c_str())) {
        return informError(updateos::UpdateError::CantDeletePreviousOS,
                           "prepareRoot %s still exists, we can't continue",
                           purefs::dir::getPreviousOSPath().c_str());
    }
    // rename the current OS to previous on partition
    informDebug("prepareRoot rename: %s->%s", currentOSPath.c_str(), previousOSPath.c_str());
    try {
        std::filesystem::rename(currentOSPath.c_str(), previousOSPath.c_str());
    }
    catch (const std::filesystem::filesystem_error &fsError) {
        return informError(updateos::UpdateError::CantRenameCurrentToPrevious,
                           "prepareRoot can't rename %s -> %s error %s",
                           purefs::dir::getCurrentOSPath().c_str(),
                           purefs::dir::getPreviousOSPath().c_str(),
                           fsError.what());
    }

    // rename the temp directory to current (extracted update)
    informDebug("prepareRoot copy: %s->%s", updateTempDirectory.c_str(), currentOSPath.c_str());
    try {
        std::filesystem::copy(
            updateTempDirectory.c_str(), currentOSPath.c_str(), std::filesystem::copy_options::recursive);
    }
    catch (const std::filesystem::filesystem_error &fsError) {
        return informError(updateos::UpdateError::CantCopyTempToCurrent,
                           "prepareRoot can't copy %s -> %s error %s",
                           updateTempDirectory.c_str(),
                           purefs::dir::getCurrentOSPath().c_str(),
                           fsError.what());
    }

    // move the contents of /sys/current/user if it exists to /user
    updateUserData();

    return updateBootJSON();
}

updateos::UpdateError UpdateMuditaOS::updateBootJSON()
{
    fs::path bootJSONAbsoulte = purefs::createPath(purefs::dir::getRootDiskPath(), purefs::file::boot_json);
    informDebug("updateBootJSON %s", bootJSONAbsoulte.c_str());

    auto *fp = std::fopen(bootJSONAbsoulte.c_str(), "r");

    if (fp != nullptr) {
        unsigned long bootJSONAbsoulteCRC = utils::filesystem::computeFileCRC32(fp);
        bootJSONAbsoulte += boot::consts::ext_crc32;

        auto *fpCRC = std::fopen(bootJSONAbsoulte.c_str(), "w");
        if (fpCRC != nullptr) {
            std::array<char, boot::consts::crc_char_size + 1> crcBuf{};
            snprintf(crcBuf.data(), crcBuf.size(), "%lX", bootJSONAbsoulteCRC);
            std::fwrite(crcBuf.data(), 1, boot::consts::crc_char_size, fpCRC);
            std::fclose(fpCRC);
        }
        else {
            return informError(updateos::UpdateError::CantUpdateCRC32JSON,
                               "Can't open %s for writing",
                               (bootJSONAbsoulte += boot::consts::ext_crc32).c_str());
        }

        std::fclose(fp);
    }
    else {
        return informError(
            updateos::UpdateError::CantUpdateCRC32JSON, "Can't open %s for reading", bootJSONAbsoulte.c_str());
    }

    informDebug("updateBootJSON no error");
    return updateos::UpdateError::NoError;
}

bool UpdateMuditaOS::unpackFileToTemp(mtar_header_t &h, unsigned long *crc32)
{
    auto readBuf            = std::make_unique<unsigned char[]>(boot::consts::tar_buf);
    const fs::path fullPath = getUpdateTmpChild(h.name);

    uint32_t blocksToRead = (h.size / boot::consts::tar_buf) + 1;
    uint32_t sizeToRead   = boot::consts::tar_buf;
    fileExtracted         = h.name;
    fileExtractedSize     = h.size;

    informUpdate(status, "Unpack %s", fullPath.filename().c_str());

    if (crc32 != nullptr) {
        *crc32 = 0;
    }
    else {
        return false;
    }

    int errCode = MTAR_ESUCCESS;
    auto *fp    = std::fopen(fullPath.c_str(), "w+");
    if (fp == nullptr) {
        informError(
            updateos::UpdateError::CantWriteToFile, "unpackFileToTemp %s can't open for writing", fullPath.c_str());
        return false;
    }

    CRC32 digest;

    for (uint32_t i = 0; i < blocksToRead; i++) {
        if (i + 1 == blocksToRead) {
            sizeToRead = h.size % boot::consts::tar_buf;
        }
        else {
            sizeToRead = boot::consts::tar_buf;
        }

        if (sizeToRead == 0)
            break;

        if ((errCode = mtar_read_data(&updateTar, readBuf.get(), sizeToRead)) != MTAR_ESUCCESS) {
            informError(
                updateos::UpdateError::CantWriteToFile, "unpackFileToTemp mtar_read_data failed, errCode=%d", errCode);
            return false;
        }

        const uint32_t dataWritten = std::fwrite(readBuf.get(), 1, sizeToRead, fp);
        if (dataWritten != sizeToRead) {
            informError(
                updateos::UpdateError::CantWriteToFile, "unpackFileToTemp %s can't write to file", fullPath.c_str());
            std::fclose(fp);
            return false;
        }

        currentExtractedBytes += dataWritten;

        digest.add(readBuf.get(), sizeToRead);
    }
    std::fclose(fp);

    *crc32 = digest.getHashValue();
    return true;
}

updateos::UpdateError UpdateMuditaOS::cleanupAfterUpdate()
{
    try {
        if (std::filesystem::is_directory(updateTempDirectory.c_str())) {
            const auto numOfFilesRemoved = std::filesystem::remove_all(updateTempDirectory.c_str());
            LOG_DEBUG("Number of files and directories removed: %lu", static_cast<unsigned long>(numOfFilesRemoved));
        }
    }
    catch (const std::filesystem::filesystem_error &e) {
        LOG_ERROR("remove_all on %s, error %s", updateTempDirectory.c_str(), e.what());
    }

    try {
        mtar_close(&updateTar);
        if (!std::filesystem::remove(updateFile)) {
            return informError(updateos::UpdateError::CantRemoveUpdateFile, "Failed to delete %s", updateFile.c_str());
        }
        LOG_DEBUG("Deleted update file %s", updateFile.c_str());
    }
    catch (const std::filesystem::filesystem_error &fsError) {
        LOG_ERROR("remove on %s, error %s", updateFile.c_str(), fsError.what());
    }

    status = updateos::UpdateState::ReadyForReset;

    informUpdateWindow();

    return updateos::UpdateError::NoError;
}

const fs::path UpdateMuditaOS::getUpdateTmpChild(const fs::path &childPath)
{
    if (childPath.string().rfind("./", 0) == 0) {
        return updateTempDirectory / childPath.string().substr(2);
    }
    else {
        return updateTempDirectory / childPath;
    }
}

updateos::UpdateError UpdateMuditaOS::prepareTempDirForUpdate(const std::filesystem::path &temporaryPath,
                                                              const std::filesystem::path &updatesOSPath)
{
    status = updateos::UpdateState::CreatingDirectories;

    if (isUpdateToBeAborted()) {
        setUpdateAbortFlag(false);
        sys::SystemManager::Reboot(owner);
        return updateos::UpdateError::UpdateAborted;
    }
    else {
        informUpdateWindow();
    }

    updateTempDirectory = temporaryPath / utils::filesystem::generateRandomId(updateos::prefix_len);

    informDebug("Temp dir for update %s", updateTempDirectory.c_str());

    if (!std::filesystem::is_directory(updatesOSPath)) {
        if (!std::filesystem::create_directory(updatesOSPath)) {
            return informError(
                updateos::UpdateError::CantCreateUpdatesDir, "%s can't create it", updatesOSPath.c_str());
        }
    }

    if (!std::filesystem::is_directory(updatesOSPath)) {
        if (!std::filesystem::create_directory(updatesOSPath)) {
            return informError(
                updateos::UpdateError::CantCreateUpdatesDir, "%s can't create it %s", updatesOSPath.c_str());
        }
        else {
            informDebug("prepareTempDirForUpdate %s created", updatesOSPath.c_str());
        }
    }
    else {
        informDebug("prepareTempDirForUpdate %s exists", updatesOSPath.c_str());
    }

    if (!std::filesystem::is_directory(temporaryPath)) {
        informDebug("prepareTempDirForUpdate %s is not a directory", temporaryPath.c_str());
        if (!std::filesystem::create_directory(temporaryPath.c_str())) {
            return informError(
                updateos::UpdateError::CantCreateTempDir, "%s can't create it %s", temporaryPath.c_str());
        }
        else {
            informDebug("prepareTempDirForUpdate %s created", temporaryPath.c_str());
        }
    }
    else {
        informDebug("prepareTempDirForUpdate %s exists", temporaryPath.c_str());
    }

    if (std::filesystem::is_directory(updateTempDirectory.c_str())) {
        informDebug("prepareTempDirForUpdate %s exists already, try to remove it", updateTempDirectory.c_str());
        try {
            std::filesystem::remove_all(updateTempDirectory.c_str());
        }
        catch (const std::filesystem::filesystem_error &e) {
            return informError(updateos::UpdateError::CantRemoveUniqueTmpDir,
                               "prepareTempDirForUpdate can't remove %s",
                               updateTempDirectory.c_str());
        }
        informDebug("prepareTempDirForUpdate %s removed", updateTempDirectory.c_str());
    }

    informDebug("prepareTempDirForUpdate trying to create %s as tempDir", updateTempDirectory.c_str());
    if (!std::filesystem::create_directory(updateTempDirectory.c_str())) {
        informError(updateos::UpdateError::CantCreateUniqueTmpDir,
                    "prepareTempDirForUpdate failed to create: %s",
                    updateTempDirectory.c_str());
    }

    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::writeBootloader(fs::path bootloaderFile)
{
    status = updateos::UpdateState::UpdatingBootloader;

#if defined(TARGET_Linux)
    return updateos::UpdateError::NoError;
#else

    if (std::filesystem::exists(bootloaderFile.c_str()) == false) {
        return informError(updateos::UpdateError::NoBootloaderFile,
                           "[Bootloader Update] File %s doesn't exist!\n",
                           bootloaderFile.c_str());
    }

    auto fileHandler = std::fopen(bootloaderFile.c_str(), "r");
    if (fileHandler == nullptr) {
        std::fclose(fileHandler);
        return informError(updateos::UpdateError::CantOpenBootloaderFile,
                           "[Bootloader Update] Failed to open file %s\n",
                           bootloaderFile.c_str());
    }

    unsigned long fileLen = std::filesystem::file_size(bootloaderFile);
    auto fileBuf          = std::make_unique<uint8_t[]>(fileLen);
    if (fileBuf == nullptr) {
        std::fclose(fileHandler);
        return informError(updateos::UpdateError::CantAllocateBuffer,
                           "[Bootloader Update] Failed to allocate buffer\n");
    }

    auto filesLoaded = std::fread(fileBuf.get(), fileLen, 1, fileHandler);
    if (filesLoaded == 0) {
        std::fclose(fileHandler);
        return informError(updateos::UpdateError::CantOpenBootloaderFile,
                           "[Bootloader Update] Failed to load file %s\n",
                           bootloaderFile.c_str());
    }
    std::fclose(fileHandler);

    informUpdate(status, "[Bootloader Update] File size: %lu B, Writing...", fileLen);

    bsp::eMMC emmc;
    emmc.Init();
    emmc.SwitchPartition(bsp::eMMC::Partition::Boot1);
    emmc.WriteBlocks(fileBuf.get(), 0, std::ceil(fileLen / FSL_SDMMC_DEFAULT_BLOCK_SIZE));

    informUpdate(status, "[Bootloader Update] DONE!\n");
    emmc.SwitchPartition(bsp::eMMC::Partition::UserArea);
    return updateos::UpdateError::NoError;
#endif
}

const json11::Json UpdateMuditaOS::getVersionInfoFromFile(const fs::path &updateFile)
{
    if (std::filesystem::exists(updateFile.c_str())) {
        mtar_t tar;
        mtar_header_t tarHeader;

        if (mtar_open(&tar, updateFile.c_str(), "r") == MTAR_EOPENFAIL) {
            LOG_WARN("%s can't open", updateFile.c_str());
            return json11::Json();
        }

        std::unique_ptr<char[]> versionFilename = std::make_unique<char[]>(boot::consts::crc_buf);
        sprintf(versionFilename.get(), "./%s", updateos::file::version);
        if (mtar_find(&tar, versionFilename.get(), &tarHeader) == MTAR_ENOTFOUND) {
            LOG_WARN("can't find %s in %s", updateos::file::version, updateFile.c_str());

            mtar_close(&tar);
            return json11::Json();
        }

        /* this file should never be larger then boot::consts::tar_buf */
        std::unique_ptr<char[]> readBuf = std::make_unique<char[]>(boot::consts::tar_buf);
        if (mtar_read_data(&tar, readBuf.get(), tarHeader.size) != MTAR_ESUCCESS) {
            LOG_WARN("can't read %s in %s", updateos::file::version, updateFile.c_str());

            return json11::Json();
        }

        mtar_close(&tar);

        std::string parserError;
        std::string dataPackage  = std::string(static_cast<char *>(readBuf.get()), tarHeader.size);
        json11::Json versionInfo = json11::Json::parse(dataPackage, parserError);
        if (parserError != "") {
            LOG_WARN("can't parse %s as JSON error: \"%s\"", updateos::file::version, parserError.c_str());
            return json11::Json();
        }

        return versionInfo;
    }
    else {
        LOG_WARN("%s does not exist", updateFile.c_str());
    }

    return json11::Json();
}

bool UpdateMuditaOS::isUpgradeToCurrent(const std::string &versionToCompare)
{
    return true;
}

const fs::path UpdateMuditaOS::checkForUpdate()
{
    const auto updatesOSPath = purefs::dir::getUpdatesOSPath();
    std::error_code errorCode;
    if (!std::filesystem::is_directory(updatesOSPath, errorCode)) {
        LOG_INFO("%s does not exist, try to create", updatesOSPath.c_str());
        std::filesystem::create_directory(updatesOSPath, errorCode);
        if (errorCode) {
            LOG_WARN("%s can't be created \"%s\"", updatesOSPath.c_str(), errorCode.message().c_str());
            return fs::path();
        }
    }

    for (auto &file : std::filesystem::directory_iterator(updatesOSPath.c_str(), errorCode)) {
        json11::Json versionInfo = UpdateMuditaOS::getVersionInfoFromFile(updatesOSPath / file.path());
        if (versionInfo.is_null())
            continue;
        if (errorCode) {
            LOG_WARN("directory_iterator for %s failed \"%s\"", updatesOSPath.c_str(), errorCode.message().c_str());
            return fs::path();
        }

        if (versionInfo[boot::json::os_version][boot::json::version_string].is_string()) {
            if (UpdateMuditaOS::isUpgradeToCurrent(
                    versionInfo[boot::json::os_version][boot::json::version_string].string_value())) {
                return updatesOSPath / file.path();
            }
        }
    }

    return fs::path();
}

updateos::UpdateError UpdateMuditaOS::updateUserData()
{
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::informError(const updateos::UpdateError errorCode, const char *format, ...)
{
    va_list argptr;
    std::unique_ptr<char[]> readBuf(new char[boot::consts::tar_buf]);
    va_start(argptr, format);
    vsnprintf(readBuf.get(), boot::consts::tar_buf, format, argptr);
    va_end(argptr);

    LOG_ERROR("UPDATE_ERRROR [%d] %s", static_cast<uint8_t>(errorCode), readBuf.get());

    auto msgToSend         = std::make_shared<sdesktop::UpdateOsMessage>(updateos::UpdateMessageType::UpdateError);
    messageText            = std::string(readBuf.get());
    msgToSend->updateStats = (updateos::UpdateStats)(*this);
    owner->bus.sendUnicast(msgToSend, app::name_desktop);

    parserFSM::Context responseContext;
    responseContext.setResponseStatus(parserFSM::http::Code::InternalServerError);
    responseContext.setEndpoint(parserFSM::EndpointType::update);
    json11::Json responseJson = json11::Json::object{{parserFSM::json::update, parserFSM::json::updateError},
                                                     {parserFSM::json::status, messageText},
                                                     {parserFSM::json::errorCode, static_cast<uint8_t>(errorCode)}};
    responseContext.setResponseBody(responseJson);
    parserFSM::MessageHandler::putToSendQueue(responseContext.createSimpleResponse());

    updateRunStatus.finishedState = status;
    updateRunStatus.finishedError = errorCode;
    return errorCode;
}

void UpdateMuditaOS::informDebug(const char *format, ...)
{
    va_list argptr;
    std::unique_ptr<char[]> readBuf(new char[boot::consts::tar_buf]);
    va_start(argptr, format);
    vsnprintf(readBuf.get(), boot::consts::tar_buf, format, argptr);
    va_end(argptr);

    LOG_DEBUG("UPDATE_DEBUG %s", readBuf.get());
}

void UpdateMuditaOS::informUpdate(const updateos::UpdateState statusCode, const char *format, ...)
{
    va_list argptr;
    std::unique_ptr<char[]> readBuf(new char[boot::consts::tar_buf]);
    va_start(argptr, format);
    vsnprintf(readBuf.get(), boot::consts::tar_buf, format, argptr);
    va_end(argptr);

    LOG_INFO("UPDATE_INFO [%d] %s", static_cast<uint8_t>(statusCode), readBuf.get());

    auto msgToSend         = std::make_shared<sdesktop::UpdateOsMessage>(updateos::UpdateMessageType::UpdateInform);
    messageText            = std::string(readBuf.get());
    msgToSend->updateStats = (updateos::UpdateStats)(*this);
    if (owner == nullptr) {
        return;
    }
    owner->bus.sendUnicast(msgToSend, app::name_desktop);

    parserFSM::Context responseContext;
    responseContext.setResponseStatus(parserFSM::http::Code::Accepted);
    responseContext.setEndpoint(parserFSM::EndpointType::update);
    json11::Json responseJson = json11::Json::object{{parserFSM::json::update, parserFSM::json::updateInfo},
                                                     {parserFSM::json::status, messageText},
                                                     {parserFSM::json::statusCode, static_cast<uint8_t>(statusCode)}};
    responseContext.setResponseBody(responseJson);
    parserFSM::MessageHandler::putToSendQueue(responseContext.createSimpleResponse());

    updateRunStatus.finishedState = status;
}

void UpdateMuditaOS::setInitialHistory(const std::string &initialHistory)
{
    LOG_DEBUG("%s", initialHistory.c_str());
    std::string parseErrors;
    updateHistory = json11::Json::parse(initialHistory, parseErrors);

    if (!parseErrors.empty() && !initialHistory.empty()) {
        LOG_WARN("Can't parse current update history, resetting");
        updateHistory = json11::Json();
    }
}

void UpdateMuditaOS::informUpdateWindow()
{
    fs::path file  = UpdateMuditaOS::checkForUpdate();
    auto msgToSend = std::make_shared<sdesktop::UpdateOsMessage>(updateos::UpdateMessageType::UpdateNow, file);
    msgToSend->updateStats.versionInformation = UpdateMuditaOS::getVersionInfoFromFile(file);
    msgToSend->updateStats.status             = status;
    if (owner != nullptr) {
        owner->bus.sendUnicast(msgToSend, app::name_desktop);
    }
}

void UpdateMuditaOS::storeRunStatusInDB()
{
    std::vector<json11::Json> tempTable;
    bool statusRunFound = false;
    if (updateHistory.is_array()) {
        for (const auto &value : updateHistory.array_items()) {
            try {
                // need to use stoul as json does not seem to handle it well
                if (std::stoul(value[updateos::settings::startTime].string_value()) == updateRunStatus.startTime) {
                    tempTable.emplace_back(updateRunStatus);

                    // this tells us we already found and element in history
                    statusRunFound = true;
                }
                else {
                    // if we found a value in history that's not ours, just copy it to temptable
                    tempTable.emplace_back(value);
                }
            }
            catch (const std::exception &arg) {
                LOG_ERROR("storeRunStatusInDB %s error - %s",
                          arg.what(),
                          value[updateos::settings::startTime].string_value().c_str());
            }
        }

        if (!statusRunFound) {
            // if our element was not found, insert it
            tempTable.emplace_back(updateRunStatus);
        }
    }
    else {
        // if the history is not a json array, initialize it
        tempTable = json11::Json::array{updateRunStatus};
    }

    updateHistory = tempTable;
    owner->storeHistory(updateHistory.dump());
}
