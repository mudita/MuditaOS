// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UpdateMuditaOS.hpp"
#include <service-desktop/ServiceDesktop.hpp>
#include <service-desktop/DesktopMessages.hpp>

#include <Service/Bus.hpp>
#include <SystemManager/SystemManager.hpp>
#include <crc32/crc32.h>
#include <json/json11.hpp>
#include <log/log.hpp>
#include <microtar/src/microtar.hpp>
#include <module-apps/application-desktop/ApplicationDesktop.hpp>
#include <purefs/filesystem_paths.hpp>
#include <vfs.hpp>

#if defined(TARGET_RT1051)
#include <board/cross/eMMC/eMMC.hpp>
#include "bsp/watchdog/watchdog.hpp"
#include "vfs_globals.hpp"
#include <module-vfs/include/user/purefs/filesystem_paths.hpp>
#endif

#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
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
}

updateos::UpdateError UpdateMuditaOS::setUpdateFile(fs::path updateFileToUse)
{
    updateFile = purefs::dir::getUpdatesOSPath() / updateFileToUse;
    if (vfs.fileExists(updateFile.c_str())) {
        versioInformation = UpdateMuditaOS::getVersionInfoFromFile(updateFile);
        if (mtar_open(&updateTar, updateFile.c_str(), "r") == MTAR_ESUCCESS) {
            totalBytes = vfs.filelength(updateTar.stream);
        }
        else {
            return informError(updateos::UpdateError::CantOpenUpdateFile, "UpdateMuditaOS::setUpdateFile can't open TAR file %s", updateFile.c_str());
        }
    }
    else {
        return informError(updateos::UpdateError::CantOpenUpdateFile, "UpdateMuditaOS::setUpdateFile %s does not exist", updateFile.c_str());
    }

    status = updateos::UpdateState::UpdateFileSet;
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::runUpdate()
{
    informDebug("Prepraring temp dir");

    updateos::UpdateError err = prepareTempDirForUpdate();
    if (err != updateos::UpdateError::NoError) {
        return informError(err, "runUpdate can't prepare temp directory for update");
    }

    informDebug("Unpacking update");
    if ((err = unpackUpdate()) == updateos::UpdateError::NoError) {
        informUpdate(status, "Unpacked");
    }
    else {
        return informError(err, "%s can't be unpacked", updateFile.c_str());
    }

    if ((err = verifyChecksums()) == updateos::UpdateError::NoError) {
        informUpdate(status, "Verify checksums");
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

    // reboot always
    sys::SystemManager::Reboot(owner);

    return err;
}

updateos::UpdateError UpdateMuditaOS::unpackUpdate()
{
    mtar_header_t tarHeader;
    filesInUpdatePackage.clear();
    status = updateos::UpdateState::ExtractingFiles;

    while ((mtar_read_header(&updateTar, &tarHeader)) != MTAR_ENULLRECORD) {
        unsigned long fileCRC32 = 0;

        if (tarHeader.type == MTAR_TDIR) {
            fs::path tmpPath = getUpdateTmpChild(tarHeader.name);
            if (vfs.mkdir(tmpPath.c_str()) != 0) {
                return informError(updateos::UpdateError::CantCreateExtractedFile, "unpackUpdate failed to create %s when extracting update tar", tmpPath.c_str());
            }
        }
        else {
            if (unpackFileToTemp(tarHeader, &fileCRC32) == false) {
                return informError(updateos::UpdateError::CantCreateExtractedFile, "unpackUpdate failed to extract update file %s", tarHeader.name);
            }
            filesInUpdatePackage.emplace_back(FileInfo(tarHeader, fileCRC32));
        }

        mtar_next(&updateTar);
    }

    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::verifyChecksums()
{
    status = updateos::UpdateState::ChecksumVerification;

    std::unique_ptr<char[]> lineBuff(
        new char[purefs::buffer::tar_buf]); // max line should be freertos max path + checksum, so this is enough
    fs::path checksumsFile = getUpdateTmpChild(updateos::file::checksums);
    vfs::FILE *fpChecksums = vfs.fopen(checksumsFile.c_str(), "r");

    if (fpChecksums == nullptr) {
        return informError(updateos::UpdateError::CantOpenChecksumsFile, "verifyChecksums can't open checksums file %s", checksumsFile.c_str());
    }

    while (!vfs.eof(fpChecksums)) {
        char *line = vfs.fgets(lineBuff.get(), purefs::buffer::tar_buf, fpChecksums);
        std::string filePath;
        unsigned long fileCRC32;

        if (lineBuff[0] == ';' || line == nullptr)
            continue;

        getChecksumInfo(line, filePath, &fileCRC32);
        unsigned long computedCRC32 = getExtractedFileCRC32(filePath);
        if (computedCRC32 != fileCRC32) {
            vfs.fclose(fpChecksums);
            return informError(updateos::UpdateError::VerifyChecksumsFailure, "verifyChecksums %s crc32 match FAIL %lX != %lX", filePath.c_str(), fileCRC32, computedCRC32);
        }
    }
    vfs.fclose(fpChecksums);
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::verifyVersion()
{
    status = updateos::UpdateState::VersionVerificiation;

    if (!vfs.fileExists(getUpdateTmpChild(updateos::file::version).c_str())) {
        return informError(updateos::UpdateError::VerifyVersionFailure, "verifyVersion %s does not exist", getUpdateTmpChild(updateos::file::version).c_str());
    }

    std::string versionJsonString = vfs.loadFileAsString(getUpdateTmpChild(updateos::file::version));
    std::string parserError;
    json11::Json updateVersionInformation = json11::Json::parse(versionJsonString, parserError);
    if (parserError != "") {
        return informError(updateos::UpdateError::VerifyVersionFailure, "verifyVersion parse json error: %s", parserError.c_str());
    }
    else {
    }
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::updateBootloader()
{
    informDebug("updateBootloader noError");
    return updateos::UpdateError::NoError;
}

unsigned long UpdateMuditaOS::getExtractedFileCRC32(const std::string &filePath)
{
    for (auto file : filesInUpdatePackage) {
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
        const std::string fileCRC32Str = infoLine.substr(lastSpacePos + 1, purefs::buffer::crc_char_size - 1);
        if (fileCRC32Long != nullptr) {
            *fileCRC32Long = strtoull(fileCRC32Str.c_str(), nullptr, purefs::buffer::crc_radix);
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
    int ret;
    // basic needed dirs

    const auto previousOSPath = purefs::dir::getPreviousOSPath();
    const auto currentOSPath  = purefs::dir::getCurrentOSPath();
    const auto userDiskPath   = purefs::dir::getUserDiskPath();

    informDebug("prepareRoot mkdir: %s", previousOSPath.c_str());
    vfs.mkdir(previousOSPath.c_str());
    informDebug("prepareRoot mkdir: %s", currentOSPath.c_str());
    vfs.mkdir(currentOSPath.c_str());
    informDebug("prepareRoot mkdir: %s", userDiskPath.c_str());
    vfs.mkdir(userDiskPath.c_str());

    // remove the previous OS version from partition
    informDebug("prepareRoot deltree: %s", previousOSPath.c_str());
    ret = vfs.deltree(previousOSPath.c_str());

    if (ret != 0) {
        informError(updateos::UpdateError::CantDeletePreviousOS,
            "prepareRoot ff_deltree on %s caused an error %s", previousOSPath.c_str(), vfs.lastErrnoToStr().c_str());
    }

    if (vfs.isDir(previousOSPath.c_str())) {
        return informError(updateos::UpdateError::CantDeletePreviousOS, "prepareRoot %s still exists, we can't continue", previousOSPath.c_str());
    }

    if (vfs.isDir(purefs::dir::getPreviousOSPath().c_str())) {
        return informError(updateos::UpdateError::CantDeletePreviousOS, "prepareRoot %s still exists, we can't continue", purefs::dir::getPreviousOSPath().c_str());
    }
    // rename the current OS to previous on partition
    informDebug("prepareRoot rename: %s->%s", currentOSPath.c_str(), previousOSPath.c_str());
    ret = vfs.rename(currentOSPath.c_str(), previousOSPath.c_str());

    if (ret != 0) {
        return informError(updateos::UpdateError::CantRenameCurrentToPrevious, "prepareRoot can't rename %s -> %s error %s",
                    purefs::dir::getCurrentOSPath().c_str(),
                    purefs::dir::getPreviousOSPath().c_str(),
                    vfs.lastErrnoToStr().c_str());
    }

    // rename the temp directory to current (extracted update)
    informDebug("prepareRoot rename: %s->%s", updateTempDirectory.c_str(), currentOSPath.c_str());
    ret = vfs.rename(updateTempDirectory.c_str(), currentOSPath.c_str());

    if (ret != 0) {
        return informError(updateos::UpdateError::CantRenameTempToCurrent, "prepareRoot can't rename %s -> %s error %s",
                    updateTempDirectory.c_str(),
                    currentOSPath.c_str(),
                    vfs.lastErrnoToStr().c_str());
    }

    // move the contents of /sys/current/user if it exists to /user
    ret = static_cast<int>(updateUserData());

    return updateBootJSON();
}

updateos::UpdateError UpdateMuditaOS::updateBootJSON()
{
    unsigned long bootJSONAbsoulteCRC = 0;
    fs::path bootJSONAbsoulte         = purefs::createPath(purefs::dir::getRootDiskPath(), purefs::file::boot_json);
    informDebug("updateBootJSON %s", bootJSONAbsoulte.c_str());

    vfs::FILE *fp = vfs.fopen(bootJSONAbsoulte.c_str(), "r");

    if (fp != nullptr) {
        vfs.computeCRC32(fp, &bootJSONAbsoulteCRC);
        bootJSONAbsoulte += purefs::extension::crc32;

        vfs::FILE *fpCRC = vfs.fopen(bootJSONAbsoulte.c_str(), "w");
        if (fpCRC != nullptr) {
            std::array<char, purefs::buffer::crc_char_size> crcBuf;
            snprintf(crcBuf.data(), crcBuf.size(), "%lX", bootJSONAbsoulteCRC);
            vfs.fwrite(crcBuf.data(), 1, purefs::buffer::crc_char_size, fpCRC);
            vfs.fclose(fpCRC);
        }
        else {
            return updateos::UpdateError::CantUpdateCRC32JSON;
        }

        vfs.fclose(fp);
    }
    else {
        return updateos::UpdateError::CantUpdateCRC32JSON;
    }

    informDebug("updateBootJSON no error");
    return updateos::UpdateError::NoError;
}

bool UpdateMuditaOS::unpackFileToTemp(mtar_header_t &h, unsigned long *crc32)
{
    std::unique_ptr<unsigned char[]> readBuf(new unsigned char[purefs::buffer::tar_buf]);
    const fs::path fullPath = getUpdateTmpChild(h.name);

    uint32_t blocksToRead = (h.size / purefs::buffer::tar_buf) + 1;
    uint32_t sizeToRead   = purefs::buffer::tar_buf;
    fileExtracted         = h.name;
    fileExtractedSize     = h.size;

    informUpdate(status, "Unpack %s", fullPath.filename().c_str());

    if (crc32 != nullptr) {
        *crc32 = 0;
    }
    else {
        return false;
    }

    int errCode   = MTAR_ESUCCESS;
    vfs::FILE *fp = vfs.fopen(fullPath.c_str(), "w+");
    if (fp == nullptr) {
        informError(updateos::UpdateError::CantWriteToFile, "unpackFileToTemp %s can't open for writing", fullPath.c_str());
        return false;
    }

    for (uint32_t i = 0; i < blocksToRead; i++) {
        if (i + 1 == blocksToRead) {
            sizeToRead = h.size % purefs::buffer::tar_buf;
        }
        else {
            sizeToRead = purefs::buffer::tar_buf;
        }

        if (sizeToRead == 0)
            break;

        if ((errCode = mtar_read_data(&updateTar, readBuf.get(), sizeToRead)) != MTAR_ESUCCESS) {
            informError(updateos::UpdateError::CantWriteToFile, "unpackFileToTemp mtar_read_data failed, errCode=%d", errCode);
            return false;
        }

        const uint32_t dataWritten = vfs.fwrite(readBuf.get(), 1, sizeToRead, fp);
        if (dataWritten != sizeToRead) {
            informError(updateos::UpdateError::CantWriteToFile,
                "unpackFileToTemp %s can't write to file error: %s", fullPath.c_str(), vfs.lastErrnoToStr().c_str());
            vfs.fclose(fp);
            return false;
        }

        currentExtractedBytes += dataWritten;

        *crc32 = Crc32_ComputeBuf(*crc32, readBuf.get(), sizeToRead);
    }
    vfs.fclose(fp);
    return true;
}

updateos::UpdateError UpdateMuditaOS::cleanupAfterUpdate()
{
    if (vfs.isDir(updateTempDirectory.c_str()) && vfs.deltree(updateTempDirectory.c_str())) {
        return informError(updateos::UpdateError::CantRemoveUniqueTmpDir, "ff_deltree failed on %s", updateTempDirectory.c_str());
    }
    mtar_close(&updateTar);
    if (vfs.remove(updateFile.c_str())) {
        return informError(updateos::UpdateError::CantRemoveUpdateFile, "Failed to delete %s", updateFile.c_str());
    }
    status = updateos::UpdateState::ReadyForReset;
    return updateos::UpdateError::NoError;
}

const fs::path UpdateMuditaOS::getUpdateTmpChild(const fs::path &childPath)
{
    return updateTempDirectory / childPath;
}

updateos::UpdateError UpdateMuditaOS::prepareTempDirForUpdate()
{
    status = updateos::UpdateState::CreatingDirectories;

    updateTempDirectory = purefs::dir::getTemporaryPath() / vfs::generateRandomId(updateos::prefix_len);

    informDebug("Temp dir for update %s", updateTempDirectory.c_str());

    if (vfs.isDir(purefs::dir::getUpdatesOSPath().c_str()) == false) {
        if (vfs.mkdir(purefs::dir::getUpdatesOSPath().c_str()) != 0) {
            return informError(updateos::UpdateError::CantCreateUpdatesDir, "%s can't create it %s", purefs::dir::getUpdatesOSPath().c_str(), vfs.lastErrnoToStr().c_str());
        }
        else {
            informDebug("prepareTempDirForUpdate %s created", purefs::dir::getUpdatesOSPath().c_str());
        }
    }
    else {
        informDebug("prepareTempDirForUpdate %s exists", purefs::dir::getUpdatesOSPath().c_str());
    }

    if (vfs.isDir(purefs::dir::getTemporaryPath().c_str()) == false) {
        informDebug("prepareTempDirForUpdate %s is not a directory", purefs::dir::getTemporaryPath().c_str());
        if (vfs.mkdir(purefs::dir::getTemporaryPath().c_str()) != 0) {
            return informError(updateos::UpdateError::CantCreateTempDir, "%s can't create it %s", purefs::dir::getTemporaryPath().c_str(), vfs.lastErrnoToStr().c_str());
        }
        else {
            informDebug("prepareTempDirForUpdate %s created", purefs::dir::getTemporaryPath().c_str());
        }
    }
    else {
        informDebug("prepareTempDirForUpdate %s exists", purefs::dir::getTemporaryPath().c_str());
    }

    if (vfs.isDir(updateTempDirectory.c_str())) {
        informDebug("prepareTempDirForUpdate %s exists already, try to remove it", updateTempDirectory.c_str());
        if (vfs.deltree(updateTempDirectory.c_str()) != 0) {
            return informError(updateos::UpdateError::CantRemoveUniqueTmpDir, "prepareTempDirForUpdate can't remove %s", updateTempDirectory.c_str());
        }
        else {
            informDebug("prepareTempDirForUpdate %s removed", updateTempDirectory.c_str());
        }
    }

    informDebug("prepareTempDirForUpdate trying to create %s as tempDir", updateTempDirectory.c_str());
    if (vfs.mkdir(updateTempDirectory.c_str()) != 0) {
        informError(updateos::UpdateError::CantCreateUniqueTmpDir, "prepareTempDirForUpdate failed to create: %s error: %s",
                    updateTempDirectory.c_str(),
                    vfs.lastErrnoToStr().c_str());
    }

    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdateMuditaOS::writeBootloader(fs::path bootloaderFile)
{
    status = updateos::UpdateState::UpdatingBootloader;

#if defined(TARGET_Linux)
    return updateos::UpdateError::NoError;
#else

    if (vfs.fileExists(bootloaderFile.c_str()) == false) {
        return informError(updateos::UpdateError::NoBootloaderFile, "[Bootloader Update] File %s doesn't exist!\n", bootloaderFile.c_str());
    }

    auto fileHandler = vfs.fopen(bootloaderFile.c_str(), "r");
    if (fileHandler == nullptr) {
        return informError(updateos::UpdateError::CantOpenBootloaderFile, "[Bootloader Update] Failed to open file %s\n", bootloaderFile.c_str());
    }

    unsigned long fileLen = vfs.filelength(fileHandler);
    auto fileBuf          = std::make_unique<uint8_t[]>(fileLen);
    if (fileBuf == nullptr) {
        return informError(updateos::UpdateError::CantAllocateBuffer, "[Bootloader Update] Failed to allocate buffer\n");
    }

    auto filesLoaded = vfs.fread(fileBuf.get(), fileLen, 1, fileHandler);
    if (filesLoaded == 0) {
        return informError(updateos::UpdateError::CantOpenBootloaderFile, "[Bootloader Update] Failed to load file %s\n", bootloaderFile.c_str());
    }

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
    if (vfs.fileExists(updateFile.c_str())) {
        mtar_t tar;
        mtar_header_t h;

        if (mtar_open(&tar, updateFile.c_str(), "r") == MTAR_EOPENFAIL) {
            LOG_INFO("UpdateMuditaOS::getVersionInfoFromFile %s can't open", updateFile.c_str());
            return json11::Json();
        }

        std::unique_ptr<char[]> versionFilename(new char[purefs::buffer::crc_buf]);
        sprintf(versionFilename.get(), "./%s", updateos::file::version);
        if (mtar_find(&tar, versionFilename.get(), &h) == MTAR_ENOTFOUND) {
            LOG_INFO("UpdateMuditaOS::getVersionInfoFromFile can't find %s in %s",
                     updateos::file::version,
                     updateFile.c_str());

            mtar_close(&tar);
            return json11::Json();
        }

        /* this file should never be larger then purefs::buffer::tar_buf */
        std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
        if (mtar_read_data(&tar, readBuf.get(), h.size) != MTAR_ESUCCESS) {
            LOG_INFO("UpdateMuditaOS::getVersionInfoFromFile can't read %s in %s",
                     updateos::file::version,
                     updateFile.c_str());

            mtar_close(&tar);
            return json11::Json();
        }

        mtar_close(&tar);

        std::string parserError;
        std::string dataPackage  = std::string(static_cast<char *>(readBuf.get()), h.size);
        json11::Json versionInfo = json11::Json::parse(dataPackage, parserError);
        if (parserError != "") {
            LOG_INFO("UpdateMuditaOS::getVersionInfoFromFile can't parse %s as JSON error: \"%s\"",
                     updateos::file::version,
                     parserError.c_str());
            return json11::Json();
        }

        return versionInfo;
    }
    else {
        LOG_INFO("UpdateMuditaOS::getVersionInfoFromFile %s does not exist", updateFile.c_str());
    }

    return json11::Json();
}

bool UpdateMuditaOS::isUpgradeToCurrent(const std::string &versionToCompare)
{
    return true;
}

const fs::path UpdateMuditaOS::checkForUpdate()
{
    const auto updatesOSPath                  = purefs::dir::getUpdatesOSPath();
    std::vector<vfs::DirectoryEntry> fileList = vfs.listdir(updatesOSPath.c_str(), updateos::extension::update, true);
    for (auto &file : fileList) {

        json11::Json versionInfo = UpdateMuditaOS::getVersionInfoFromFile(updatesOSPath / file.fileName);
        if (versionInfo.is_null())
            continue;

        if (versionInfo[purefs::json::os_version][purefs::json::version_string].is_string()) {
            if (UpdateMuditaOS::isUpgradeToCurrent(
                    versionInfo[purefs::json::os_version][purefs::json::version_string].string_value())) {
                return updatesOSPath / file.fileName;
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
    std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
    va_start(argptr, format);
    vsnprintf(readBuf.get(), purefs::buffer::tar_buf, format, argptr);
    va_end(argptr);

    LOG_ERROR("UPDATE_ERRROR [%d] %s", static_cast<uint8_t>(errorCode), readBuf.get());

    auto msgToSend         = std::make_shared<sdesktop::UpdateOsMessage>(updateos::UpdateMessageType::UpdateError);
    messageText            = std::string(readBuf.get());
    msgToSend->updateStats = (updateos::UpdateStats)(*this);
    sys::Bus::SendUnicast(msgToSend, app::name_desktop, owner);

    parserFSM::Context response;
    response.setResponseStatus(parserFSM::http::Code::InternalServerError);
    response.setEndpoint(parserFSM::EndpointType::update);
    json11::Json responseJson = json11::Json::object
        {
            { parserFSM::json::update,  parserFSM::json::updateError},
            { parserFSM::json::status, messageText },
            { parserFSM::json::errorCode, static_cast<uint8_t>(errorCode) }
        };

    response.setResponseBody(responseJson);
    parserFSM::MessageHandler::putToSendQueue(response.createSimpleResponse());

    return errorCode;
}

void UpdateMuditaOS::informDebug(const char *format, ...)
{
    va_list argptr;
    std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
    va_start(argptr, format);
    vsnprintf(readBuf.get(), purefs::buffer::tar_buf, format, argptr);
    va_end(argptr);

    LOG_DEBUG("UPDATE_DEBUG %s", readBuf.get());
}

void UpdateMuditaOS::informUpdate(const updateos::UpdateState statusCode, const char *format, ...)
{
    va_list argptr;
    std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
    va_start(argptr, format);
    vsnprintf(readBuf.get(), purefs::buffer::tar_buf, format, argptr);
    va_end(argptr);

    LOG_INFO("UPDATE_INFO [%d] %s", static_cast<uint8_t>(statusCode), readBuf.get());

    auto msgToSend         = std::make_shared<sdesktop::UpdateOsMessage>(updateos::UpdateMessageType::UpdateInform);
    messageText            = std::string(readBuf.get());
    msgToSend->updateStats = (updateos::UpdateStats)(*this);
    sys::Bus::SendUnicast(msgToSend, app::name_desktop, owner);

    parserFSM::Context response;
    response.setResponseStatus(parserFSM::http::Code::Accepted);
    response.setEndpoint(parserFSM::EndpointType::update);
    json11::Json responseJson = json11::Json::object
        {
            { parserFSM::json::update,  parserFSM::json::updateInfo},
            { parserFSM::json::status, messageText },
            { parserFSM::json::statusCode, static_cast<uint8_t>(statusCode) }
        };
    response.setResponseBody(responseJson);
    parserFSM::MessageHandler::putToSendQueue(response.createSimpleResponse());
}
