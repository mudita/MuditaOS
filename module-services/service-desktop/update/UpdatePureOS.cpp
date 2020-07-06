#include "UpdatePureOS.hpp"
#include <service-desktop/ServiceDesktop.hpp>
#include <source/version.hpp>
#include <time/time_conversion.hpp>
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

UpdatePureOS::UpdatePureOS(ServiceDesktop *ownerService) : owner(ownerService)
{}

updateos::UpdateError UpdatePureOS::setUpdateFile(fs::path updateFileToUse)
{
    LOG_DEBUG("UpdatePureOS::setUpdateFile updateFileToUse:%s", updateFileToUse.c_str());
    updateFile = purefs::dir::os_updates / updateFileToUse;
    LOG_DEBUG("UpdatePureOS::setUpdateFile updateFile:%s", updateFile.c_str());
    if (vfs.fileExists(updateFile.c_str())) {
        if (mtar_open(&updateTar, updateFile.c_str(), "r") == MTAR_ESUCCESS) {
            LOG_INFO("UpdatePureOS::setUpdateFile TAR_FILE: %s opened", updateFile.c_str());
        }
        else {
            LOG_ERROR("UpdatePureOS::setUpdateFile can't open TAR file %s", updateFile.c_str());
            return updateos::UpdateError::CantOpenUpdateFile;
        }
    }
    else {
        LOG_ERROR("UpdatePureOS::setUpdateFile %s does not exist", updateFile.c_str());
        return updateos::UpdateError::CantOpenUpdateFile;
    }
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdatePureOS::runUpdate()
{
    LOG_INFO("runUpdate updateFile:%s", updateFile.c_str());

    updateos::UpdateError err = prepareTempDirForUpdate();
    if (err != updateos::UpdateError::NoError) {
        LOG_ERROR("runUpdate can't prepare temp directory for update");
        return err;
    }

    if ((err = unpackUpdate()) == updateos::UpdateError::NoError) {
        LOG_INFO("runUpdate %s unpacked", updateFile.c_str());
    }
    else {
        LOG_ERROR("runUpdate %s can't be unpacked", updateFile.c_str());
        return err;
    }

    if ((err = verifyChecksums()) == updateos::UpdateError::NoError) {
        LOG_INFO("runUpdate %s verifyChecksums success", updateFile.c_str());
    }
    else {
        LOG_ERROR("runUpdate %s checksum verification failed", updateFile.c_str());
        return err;
    }

    if ((err = prepareRoot()) == updateos::UpdateError::NoError) {
        LOG_INFO("runUpdate %s root ready for reset", updateFile.c_str());
    }
    else {
        LOG_ERROR("runUpdate %s can't prepare root dir for reset", updateFile.c_str());
    }

    return cleanupAfterUpdate();
}

updateos::UpdateError UpdatePureOS::unpackUpdate()
{
    mtar_header_t h;
    filesInUpdatePackage.clear();
    while ((mtar_read_header(&updateTar, &h)) != MTAR_ENULLRECORD) {
        LOG_DEBUG("unpackUpdate %s (%d bytes) type:%s", h.name, h.size, (h.type == MTAR_TDIR) ? "dir" : "file");
        unsigned long fileCRC32 = 0;
        if (h.type == MTAR_TDIR) {
            fs::path tmpPath = getUpdateTmpChild(h.name);
            if (vfs.mkdir(tmpPath.c_str()) != 0) {
                LOG_ERROR("unpackUpdate failed to create %s when extracting update tar", tmpPath.c_str());
                return (updateos::UpdateError::CantCreateExtractedFile);
            }
        }
        else {
            if (unpackFileToTemp(h, &fileCRC32) == false) {
                LOG_ERROR("unpackUpdate failed to extract update file %s", h.name);
                return (updateos::UpdateError::CantCreateExtractedFile);
            }

            filesInUpdatePackage.emplace_back(FileInfo(h, fileCRC32));
        }
        mtar_next(&updateTar);
    }
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdatePureOS::verifyChecksums()
{
    std::unique_ptr<char[]> lineBuff(
        new char[purefs::buffer::tar_buf]); // max line should be freertos max path + checksum, so this is enough
    fs::path checksumsFile = getUpdateTmpChild(updateos::file::checksums);
    vfs::FILE *fpChecksums = vfs.fopen(checksumsFile.c_str(), "r");

    if (fpChecksums == nullptr) {
        LOG_ERROR("verifyChecksums can't open checksums file %s", checksumsFile.c_str());
        return updateos::UpdateError::CantOpenChecksumsFile;
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
            LOG_ERROR("verifyChecksums %s crc32 match FAIL %lX != %lX", filePath.c_str(), fileCRC32, computedCRC32);
            vfs.fclose(fpChecksums);
            return updateos::UpdateError::VerifyChecksumsFailure;
        }
    }
    vfs.fclose(fpChecksums);

    LOG_DEBUG("verifyChecksums noError");
    return updateos::UpdateError::NoError;
}

unsigned long UpdatePureOS::getExtractedFileCRC32(const std::string &filePath)
{
    for (auto file : filesInUpdatePackage) {
        if (file.fileName == filePath) {
            return file.fileCRC32;
        }
    }
    return 0;
}

void UpdatePureOS::getChecksumInfo(const std::string &infoLine, std::string &filePath, unsigned long *fileCRC32Long)
{
    std::size_t lastSpacePos = infoLine.find_last_of(' ');
    if (lastSpacePos > 0) {
        filePath                       = infoLine.substr(0, lastSpacePos);
        const std::string fileCRC32Str = infoLine.substr(lastSpacePos + 1, purefs::buffer::crc_char_size - 1);
        if (fileCRC32Long != nullptr) {
            *fileCRC32Long = strtoull(fileCRC32Str.c_str(), nullptr, purefs::buffer::crc_radix);
            LOG_DEBUG("getChecksumInfo filePath: %s fileCRC32Str: %s fileCRC32Long: %lu",
                      filePath.c_str(),
                      fileCRC32Str.c_str(),
                      *fileCRC32Long);
        }
    }
}

updateos::UpdateError UpdatePureOS::prepareRoot()
{
    LOG_INFO("prepareRoot()");
    int ret;
    // basic needed dirs

    LOG_DEBUG("prepareRoot mkdir: %s", purefs::dir::os_previous.c_str());
    vfs.mkdir(purefs::dir::os_previous.c_str());
    LOG_DEBUG("prepareRoot mkdir: %s", purefs::dir::os_current.c_str());
    vfs.mkdir(purefs::dir::os_current.c_str());
    LOG_DEBUG("prepareRoot mkdir: %s", purefs::dir::user_disk.c_str());
    vfs.mkdir(purefs::dir::user_disk.c_str());

    // remove the previous OS version from partition
    LOG_DEBUG("prepareRoot deltree: %s", purefs::dir::os_previous.c_str());
    ret = vfs.deltree(purefs::dir::os_previous.c_str());

    if (ret != 0) {
        LOG_ERROR("prepareRoot ff_deltree on %s caused an error %s",
                  purefs::dir::os_previous.c_str(),
                  vfs.lastErrnoToStr().c_str());
    }

    if (vfs.isDir(purefs::dir::os_previous.c_str())) {
        LOG_ERROR("prepareRoot %s still exists, we can't continue", purefs::dir::os_previous.c_str());
        return updateos::UpdateError::CantDeletePreviousOS;
    }
    // rename the current OS to previous on partition
    LOG_DEBUG("prepareRoot rename: %s->%s", purefs::dir::os_current.c_str(), purefs::dir::os_previous.c_str());
    ret = vfs.rename(purefs::dir::os_current.c_str(), purefs::dir::os_previous.c_str());

    if (ret != 0) {
        LOG_ERROR("prepareRoot can't rename %s -> %s error %s",
                  purefs::dir::os_current.c_str(),
                  purefs::dir::os_previous.c_str(),
                  vfs.lastErrnoToStr().c_str());
        return updateos::UpdateError::CantRenameCurrentToPrevious;
    }

    // rename the temp directory to current (extracted update)
    LOG_DEBUG("prepareRoot rename: %s->%s", updateTempDirectory.c_str(), purefs::dir::os_current.c_str());
    ret = vfs.rename(updateTempDirectory.c_str(), purefs::dir::os_current.c_str());

    if (ret != 0) {
        LOG_ERROR("prepareRoot can't rename %s -> %s error %s",
                  updateTempDirectory.c_str(),
                  purefs::dir::os_current.c_str(),
                  vfs.lastErrnoToStr().c_str());
        return updateos::UpdateError::CantRenameTempToCurrent;
    }
    // make sure that boot.ini points to the current version

    return updateBootINI();
}

bool UpdatePureOS::iniSet(sbini_t *ini, const std::string section, const std::string name, const std::string value)
{
    if (sbini_set_string(ini, section.c_str(), name.c_str(), value.c_str())) {
        LOG_ERROR("iniSet can't update internal structure %s::%s::%s", section.c_str(), name.c_str(), value.c_str());
        sbini_free(ini);
        return false;
    }
    return true;
}

updateos::UpdateError UpdatePureOS::updateBootINI()
{
    unsigned long bootIniAbsoulteCRC = 0;
    fs::path bootIniAbsoulte         = purefs::dir::eMMC_disk / purefs::file::boot_ini;
    LOG_DEBUG("updateBootINI");
    sbini_t *ini = sbini_load(bootIniAbsoulte.c_str());

    if (ini == NULL) {
        LOG_INFO("updateBootINI can't load %s", purefs::file::boot_ini.c_str());
        ini = sbini_new();
    }

    /* ini will be freed in iniSet if it fails */
    if (!iniSet(ini, purefs::ini::main, purefs::ini::os_type, PATH_CURRENT)) {
        return updateos::UpdateError::CantUpdateINI;
    }
    if (!iniSet(ini, purefs::ini::main, purefs::ini::os_image, purefs::file::boot_bin.c_str())) {
        return updateos::UpdateError::CantUpdateINI;
    }
    if (!iniSet(ini, purefs::ini::main, purefs::ini::timestamp, std::to_string(utils::time::Time().getTime()))) {
        return updateos::UpdateError::CantUpdateINI;
    }
    if (sbini_save(ini, bootIniAbsoulte.c_str())) {
        LOG_ERROR("updateBootINI can't save ini file at: %s", bootIniAbsoulte.c_str());
        sbini_free(ini);
        return updateos::UpdateError::CantSaveINI;
    }

    sbini_free(ini);

    vfs::FILE *fp = vfs.fopen(bootIniAbsoulte.c_str(), "r");
    if (fp != nullptr) {
        vfs.computeCRC32(fp, &bootIniAbsoulteCRC);
        bootIniAbsoulte += purefs::extension::crc32;

        vfs::FILE *fpCRC = vfs.fopen(bootIniAbsoulte.c_str(), "w");
        if (fpCRC != nullptr) {
            std::array<char, purefs::buffer::crc_char_size> crcBuf;
            sprintf(crcBuf.data(), "%lX", bootIniAbsoulteCRC);
            vfs.fwrite(crcBuf.data(), 1, purefs::buffer::crc_char_size, fpCRC);
            vfs.fclose(fpCRC);
        }
        vfs.fclose(fp);
    }

    LOG_DEBUG("updateBootINI no error");
    return updateos::UpdateError::NoError;
}

bool UpdatePureOS::unpackFileToTemp(mtar_header_t &h, unsigned long *crc32)
{
    std::unique_ptr<unsigned char[]> readBuf(new unsigned char[purefs::buffer::tar_buf]);
    const fs::path fullPath = getUpdateTmpChild(h.name);

    LOG_DEBUG("unpackFileToTemp %s", fullPath.c_str());

    uint32_t blocksToRead = (h.size / purefs::buffer::tar_buf) + 1;
    uint32_t sizeToRead   = purefs::buffer::tar_buf;

    if (crc32 != nullptr) {
        *crc32 = 0;
    }
    else {
        return false;
    }

    int errCode   = MTAR_ESUCCESS;
    vfs::FILE *fp = vfs.fopen(fullPath.c_str(), "w+");
    if (fp == nullptr) {
        LOG_ERROR("unpackFileToTemp %s can't open for writing", fullPath.c_str());
        return false;
    }

    for (uint32_t i = 0; i < blocksToRead; i++) {
        if (i + 1 == blocksToRead) {
            if (h.size < purefs::buffer::tar_buf) {
                sizeToRead = h.size;
            }
            else {
                sizeToRead = updateTar.remaining_data;
            }
        }

        if ((errCode = mtar_read_data(&updateTar, readBuf.get(), sizeToRead)) != MTAR_ESUCCESS) {
            LOG_ERROR("unpackFileToTemp mtar_read_data failed, errCode=%d", errCode);
            return false;
        }

        const uint32_t dataWritten = vfs.fwrite(readBuf.get(), 1, sizeToRead, fp);
        if (dataWritten != sizeToRead) {
            LOG_ERROR(
                "unpackFileToTemp %s can't write to file error: %s", fullPath.c_str(), vfs.lastErrnoToStr().c_str());
            vfs.fclose(fp);
            return false;
        }

        *crc32 = Crc32_ComputeBuf(*crc32, readBuf.get(), sizeToRead);
    }
    vfs.fclose(fp);
    return true;
}

updateos::UpdateError UpdatePureOS::cleanupAfterUpdate()
{
    if (vfs.isDir(updateTempDirectory.c_str()) && vfs.deltree(updateTempDirectory.c_str())) {
        LOG_ERROR("ff_deltree failed on %s", updateTempDirectory.c_str());
        return updateos::UpdateError::CantRemoveUniqueTmpDir;
    }
    return updateos::UpdateError::NoError;
}

const fs::path UpdatePureOS::getUpdateTmpChild(const fs::path &childPath)
{
    return updateTempDirectory / childPath;
}

updateos::UpdateError UpdatePureOS::prepareTempDirForUpdate()
{
    updateTempDirectory = purefs::dir::tmp / vfs::generateRandomId(updateos::prefix_len);

    LOG_DEBUG("prepareTempDirForUpdate %s", updateTempDirectory.c_str());

    if (vfs.isDir(purefs::dir::os_updates.c_str()) == false) {
        LOG_DEBUG("prepareTempDirForUpdate %s is not a directory", purefs::dir::os_updates.c_str());
        if (vfs.mkdir(purefs::dir::os_updates.c_str()) != 0) {
            LOG_ERROR("%s can't create it %s", purefs::dir::os_updates.c_str(), vfs.lastErrnoToStr().c_str());
            return updateos::UpdateError::CantCreateUpdatesDir;
        }
        else {
            LOG_DEBUG("prepareTempDirForUpdate %s created", purefs::dir::os_updates.c_str());
        }
    }
    else {
        LOG_DEBUG("prepareTempDirForUpdate %s exists", purefs::dir::os_updates.c_str());
    }

    if (vfs.isDir(purefs::dir::tmp.c_str()) == false) {
        LOG_INFO("prepareTempDirForUpdate %s is not a directory", purefs::dir::tmp.c_str());
        if (vfs.mkdir(purefs::dir::tmp.c_str()) != 0) {
            LOG_ERROR("%s can't create it %s", purefs::dir::tmp.c_str(), vfs.lastErrnoToStr().c_str());
            return updateos::UpdateError::CantCreateTempDir;
        }
        else {
            LOG_DEBUG("prepareTempDirForUpdate %s created", purefs::dir::tmp.c_str());
        }
    }
    else {
        LOG_DEBUG("prepareTempDirForUpdate %s exists", purefs::dir::tmp.c_str());
    }

    if (vfs.isDir(updateTempDirectory.c_str())) {
        LOG_DEBUG("prepareTempDirForUpdate %s exists already, try to remove it", updateTempDirectory.c_str());
        if (vfs.deltree(updateTempDirectory.c_str()) != 0) {
            LOG_ERROR("prepareTempDirForUpdate can't remove %s", updateTempDirectory.c_str());
            return updateos::UpdateError::CantRemoveUniqueTmpDir;
        }
        else {
            LOG_DEBUG("prepareTempDirForUpdate %s removed", updateTempDirectory.c_str());
        }
    }

    LOG_DEBUG("prepareTempDirForUpdate trying to create %s as tempDir", updateTempDirectory.c_str());
    if (vfs.mkdir(updateTempDirectory.c_str()) != 0) {
        LOG_ERROR("prepareTempDirForUpdate failed to create: %s error: %s",
                  updateTempDirectory.c_str(),
                  vfs.lastErrnoToStr().c_str());
        return updateos::UpdateError::CantCreateUniqueTmpDir;
    }

    LOG_INFO("prepareTempDirForUpdate tempDir selected %s", updateTempDirectory.c_str());
    return updateos::UpdateError::NoError;
}
