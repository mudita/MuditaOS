#include "UpdatePureOS.h"

#include "module-utils/time/ScopedTime.hpp"
#include "service-desktop/ServiceDesktop.hpp"
#include "module-vfs/vfs.hpp"

using namespace updateos;

static bool endsWith(std::string const &s, std::string const &suffix)
{
    return s.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

UpdatePureOS::UpdatePureOS(ServiceDesktop *ownerService) : owner(ownerService)
{}

updateos::UpdateError UpdatePureOS::runUpdate(const fs::path fileName)
{
    LOG_INFO("runUpdate fileName:%s", fileName.c_str());

    UpdateError err = prepareTempDirForUpdate();
    if (err != UpdateError::NoError) {
        LOG_ERROR("runUpdate can't prepare temp directory for update");
        return (err);
    }
    int ret;

    fs::path tarFileName(dir::updates / fileName);

    if ((ret = mtar_open(&updateTar, tarFileName.c_str(), "r")) == MTAR_ESUCCESS) {
        LOG_INFO("runUpdate TAR_FILE: %s opened", tarFileName.c_str());
        if ((err = unpackUpdate()) == UpdateError::NoError) {
            LOG_INFO("runUpdate %s unpacked", tarFileName.c_str());
        }
        else {
            LOG_ERROR("runUpdate %s can't be unpacked", tarFileName.c_str());
            return (err);
        }

        if ((err = verifyChecksums()) == UpdateError::NoError) {
            LOG_INFO("runUpdate %s verifyChecksums success", tarFileName.c_str());
        }
        else {
            LOG_ERROR("runUpdate %s checksum verification failed", tarFileName.c_str());
            return (err);
        }

        if ((err = prepareRoot()) == UpdateError::NoError) {
            LOG_INFO("runUpdate %s root ready for reset", tarFileName.c_str());
        }
        else {
            LOG_ERROR("runUpdate %s can't prepare root dir for reset", tarFileName.c_str());
        }
    }
    else {
        LOG_ERROR("runUpdate %s open error %s", tarFileName.c_str(), mtar_strerror(ret));
        return (UpdateError::CantOpenUpdateFile);
    }

    return (UpdateError::NoError);
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
            if (ff_mkdir(tmpPath.c_str()) != 0) {
                LOG_ERROR("unpackAndMove failed to create %s when extracting update tar", tmpPath.c_str());
                return (UpdateError::CantCreateExtractedFile);
            }
        }
        else {
            if (unpackFileToTemp(h, &fileCRC32) == false) {
                LOG_ERROR("unpackAndMove failed to extract update file %s", h.name);
                return (UpdateError::CantCreateExtractedFile);
            }

            filesInUpdatePackage.push_back(FileInfo(h, fileCRC32));
        }
        mtar_next(&updateTar);
    }
    return (UpdateError::NoError);
}

updateos::UpdateError UpdatePureOS::verifyChecksums()
{
    std::unique_ptr<char[]> lineBuff(
        new char[purefs::buffer::tar_buf]); // max line should be freertos max path + checksum, so this is enough
    fs::path checksumsFile = getUpdateTmpChild(file::checksums);
    FF_FILE *fpChecksums   = ff_fopen(checksumsFile.c_str(), "r");
    if (fpChecksums != NULL) {
        while (!ff_feof(fpChecksums)) {
            char *line = ff_fgets(lineBuff.get(), purefs::buffer::tar_buf, fpChecksums);

            if (line) {
                if (lineBuff[0] == ';') { // lines that start with ; are comments
                    continue;
                }
                else {
                    std::string filePath;
                    unsigned long fileCRC32;

                    getChecksumInfo(line, filePath, &fileCRC32);
                    unsigned long computedCRC32 = getExtractedFileCRC32(filePath);
                    if (computedCRC32 != fileCRC32) {
                        LOG_ERROR("verifyChecksums %s crc32 match FAIL %lX != %lX",
                                  filePath.c_str(),
                                  fileCRC32,
                                  computedCRC32);
                        ff_fclose(fpChecksums);
                        return (UpdateError::VerifyChecksumsFailure);
                    }
                    else {
                        LOG_DEBUG(
                            "verifyChecksums %s crc32 match OK %lX == %lX", filePath.c_str(), fileCRC32, computedCRC32);
                    }
                }
            }
        }
        ff_fclose(fpChecksums);
    }
    else {
        LOG_ERROR("verifyChecksums can't open checksums file %s", checksumsFile.c_str());
        return (UpdateError::CantOpenChecksumsFile);
    }
    return (UpdateError::NoError);
}

unsigned long UpdatePureOS::getExtractedFileCRC32(const std::string filePath)
{
    for (auto file : filesInUpdatePackage) {
        if (file.fileName == filePath) {
            return (file.fileCRC32);
        }
    }
    return (0);
}

void UpdatePureOS::getChecksumInfo(const std::string infoLine, std::string &filePath, unsigned long *fileCRC32Long)
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
    ff_mkdir(purefs::dir::os_previous.c_str());
    ff_mkdir(purefs::dir::os_current.c_str());
    ff_mkdir(purefs::dir::user_disk.c_str());

    // remove the previous OS version from partition
    ret = ff_deltree(purefs::dir::os_previous.c_str());

    if (ret != 0) {
        LOG_ERROR("prepareRoot ff_deltree on %s caused an error %s",
                  purefs::dir::os_previous.c_str(),
                  strerror(stdioGET_ERRNO()));
    }

    if (isDirectory(purefs::dir::os_previous)) {
        LOG_ERROR("prepareRoot %s still exists, we can't continue", purefs::dir::os_previous.c_str());
        return (UpdateError::CantDeletePreviousOS);
    }
    // rename the current OS to previous on partition
    ret = ff_rename(purefs::dir::os_current.c_str(), purefs::dir::os_previous.c_str(), true);

    if (ret != 0) {
        LOG_ERROR("prepareRoot can't rename %s -> %s error %s",
                  purefs::dir::os_current.c_str(),
                  purefs::dir::os_previous.c_str(),
                  strerror(stdioGET_ERRNO()));
        return (UpdateError::CantRenameCurrentToPrevious);
    }

    // rename the temp directory to current (extracted update)
    ret = ff_rename(updateTempDirectory.c_str(), purefs::dir::os_current.c_str(), true);

    if (ret != 0) {
        LOG_ERROR("prepareRoot can't rename %s -> %s error %s",
                  updateTempDirectory.c_str(),
                  purefs::dir::os_current.c_str(),
                  strerror(stdioGET_ERRNO()));
        return (UpdateError::CantRenameTempToCurrent);
    }
    // make sure that boot.ini points to the current version

    return (updateBootINI());
}

updateos::UpdateError UpdatePureOS::updateBootINI()
{
    sbini_t *ini = sbini_load(purefs::file::boot_ini.c_str());
    if (!ini) {
        LOG_ERROR("updateBootINI can't open %s", purefs::file::boot_ini.c_str());
    }
    return (UpdateError::NoError);
}

bool UpdatePureOS::unpackFileToTemp(mtar_header_t &h, unsigned long *crc32)
{
    std::unique_ptr<unsigned char[]> readBuf(new unsigned char[purefs::buffer::tar_buf]);
    const fs::path fullPath = getUpdateTmpChild(h.name);

    LOG_DEBUG("unpackFileToTemp %s", fullPath.c_str());

    uint32_t blocksToRead = (h.size / purefs::buffer::tar_buf) + 1;
    uint32_t sizeToRead   = purefs::buffer::tar_buf;

    if (crc32 != nullptr)
        *crc32 = 0;

    int errCode = MTAR_ESUCCESS;
    FF_FILE *fp = ff_fopen(fullPath.c_str(), "w+");
    if (fp == NULL) {
        LOG_ERROR("unpackFileToTemp %s can't open for reading", fullPath.c_str());
        return (false);
    }
    auto time = utils::time::Scoped(std::string(fullPath));

    for (uint32_t i = 0; i < blocksToRead; i++) {
        if (i + 1 == blocksToRead) {
            if (h.size < purefs::buffer::tar_buf)
                sizeToRead = h.size;
            else
                sizeToRead = updateTar.remaining_data;
        }

        if ((errCode = mtar_read_data(&updateTar, readBuf.get(), sizeToRead)) != MTAR_ESUCCESS) {
            LOG_ERROR("unpackFileToTemp mtar_read_data failed, errCode=%d", errCode);
            return (false);
        }

        const uint32_t dataWritten = ff_fwrite(readBuf.get(), 1, sizeToRead, fp);
        if (dataWritten != sizeToRead) {
            LOG_ERROR(
                "unpackFileToTemp %s can't write to file error: %s", fullPath.c_str(), strerror(stdioGET_ERRNO()));
            ff_fclose(fp);
            return (false);
        }

        if (crc32 != nullptr)
            *crc32 = Crc32_ComputeBuf(*crc32, readBuf.get(), sizeToRead);
    }
    ff_fclose(fp);
    return (true);
}

void UpdatePureOS::cleanupAfterUpdate()
{
    if (isDirectory(updateTempDirectory)) {
        if (ff_deltree(updateTempDirectory.c_str()) != 0) {
            LOG_ERROR("ff_deltree failed on %s", updateTempDirectory.c_str());
        }
    }
}

const fs::path UpdatePureOS::getUpdateTmpChild(const fs::path childPath)
{
    return (updateTempDirectory / childPath);
}

updateos::UpdateError UpdatePureOS::prepareTempDirForUpdate()
{
    updateTempDirectory = dir::tmp / vfs::generateRandomId(prefix_len);

    LOG_DEBUG("prepareTempDirForUpdate %s", updateTempDirectory.c_str());

    if (isDirectory(dir::updates) == false) {
        LOG_DEBUG("prepareTempDirForUpdate %s is not a directory", dir::updates.c_str());
        if (ff_mkdir(dir::updates.c_str()) != 0) {
            LOG_ERROR("%s can't create it %s", dir::updates.c_str(), strerror(stdioGET_ERRNO()));
            return (UpdateError::CantCreateUpdatesDir);
        }
        else {
            LOG_DEBUG("prepareTempDirForUpdate %s created", dir::updates.c_str());
        }
    }
    else {
        LOG_DEBUG("prepareTempDirForUpdate %s exists", dir::updates.c_str());
    }

    if (isDirectory(dir::tmp) == false) {
        LOG_INFO("prepareTempDirForUpdate %s is not a directory", dir::tmp.c_str());
        if (ff_mkdir(dir::tmp.c_str()) != 0) {
            LOG_ERROR("%s can't create it %s", dir::tmp.c_str(), strerror(stdioGET_ERRNO()));
            return (UpdateError::CantCreateTempDir);
        }
        else {
            LOG_DEBUG("prepareTempDirForUpdate %s created", dir::tmp.c_str());
        }
    }
    else {
        LOG_DEBUG("prepareTempDirForUpdate %s exists", dir::tmp.c_str());
    }

    if (isDirectory(updateTempDirectory)) {
        LOG_DEBUG("prepareTempDirForUpdate %s exists already, try to remove it", updateTempDirectory.c_str());
        if (ff_deltree(updateTempDirectory.c_str()) != 0) {
            LOG_ERROR("prepareTempDirForUpdate can't remove %s", updateTempDirectory.c_str());
            return (UpdateError::CantRemoveUniqueTmpDir);
        }
        else {
            LOG_DEBUG("prepareTempDirForUpdate %s removed", updateTempDirectory.c_str());
        }
    }

    LOG_DEBUG("prepareTempDirForUpdate trying to create %s as tempDir", updateTempDirectory.c_str());
    if (ff_mkdir(updateTempDirectory.c_str()) != 0) {
        LOG_ERROR("prepareTempDirForUpdate failed to create: %s error: %s",
                  updateTempDirectory.c_str(),
                  strerror(stdioGET_ERRNO()));
        return (UpdateError::CantCreateUniqueTmpDir);
    }

    LOG_INFO("prepareTempDirForUpdate tempDir selected %s", updateTempDirectory.c_str());
    return (UpdateError::NoError);
}

bool UpdatePureOS::fileExists(const fs::path pathToCheck)
{
    FF_Stat_t fileStatus;
    const int ret = ff_stat(pathToCheck.c_str(), &fileStatus);
    if (ret == 0) {
        return (true);
    }
    return (false);
}

bool UpdatePureOS::isDirectory(const fs::path pathToCheck)
{
    if (fileExists(pathToCheck)) {
        FF_Stat_t fileStatus;
        const int ret = ff_stat(pathToCheck.c_str(), &fileStatus);
        if (ret == 0) {
            return (fileStatus.st_mode == FF_IFDIR);
        }
        else {
            return (false);
        }
    }
    return (false);
}

const json11::Json UpdatePureOS::getUpdateFileList()
{
    FF_FindData_t *pxFindStruct = nullptr;
    std::vector<FileInfo> updateFiles;
    pxFindStruct = (FF_FindData_t *)pvPortMalloc(sizeof(FF_FindData_t));
    memset(pxFindStruct, 0x00, sizeof(FF_FindData_t));

    if (ff_findfirst(updateos::dir::updates.c_str(), pxFindStruct) == 0) {
        do {
            /* Print the files name, size, and attribute string. */
            LOG_INFO("%s", pxFindStruct->pcFileName);
            if (endsWith(pxFindStruct->pcFileName, updateos::extension::update)) {
                updateFiles.push_back(pxFindStruct);
            }
        } while (ff_findnext(pxFindStruct) == 0);
    }

    vPortFree(pxFindStruct);
    return (json11::Json(updateFiles));
}
