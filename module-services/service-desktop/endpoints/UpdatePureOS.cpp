//
// Created by atom on 3/19/20.
//

#include "UpdatePureOS.h"
#include "../ServiceDesktop.hpp"

static bool endsWith(std::string const &s, std::string const &suffix)
{
    return s.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

UpdatePureOS::UpdatePureOS(ServiceDesktop *ownerService) : owner(ownerService)
{}

std::string UpdatePureOS::generateRandomId(size_t length = 0)
{
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    generator.seed(utils::time::Time().getTime());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string random_string;

    for (std::size_t i = 0; i < length; ++i) {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}

updateos::UpdateError UpdatePureOS::runUpdate(const fs::path fileName)
{
    LOG_INFO("runUpdate fileName:%s", fileName.c_str());
    updateos::UpdateError err = prepareTempDirForUpdate();
    if (err != updateos::NoError) {
        LOG_ERROR("runUpdate can't prepare temp directory for update");
        return (err);
    }
    int ret;

    fs::path tarFileName(updateos::dir::updates / fileName);

    if ((ret = mtar_open(&updateTar, tarFileName.c_str(), "r")) == MTAR_ESUCCESS) {
        LOG_INFO("runUpdate TAR_FILE: %s opened", tarFileName.c_str());
        if ((err = unpackUpdate()) == updateos::NoError) {
            LOG_INFO("runUpdate %s unpacked", tarFileName.c_str());
        }
        else {
            LOG_ERROR("runUpdate %s can't be unpacked", tarFileName.c_str());
            return (err);
        }

        if ((err = verifyChecksums()) == updateos::NoError) {
            LOG_INFO("runUpdate %s verifyChecksums success", tarFileName.c_str());
        }
        else {
            LOG_ERROR("runUpdate %s checksum verification failed", tarFileName.c_str());
            return (err);
        }

        if ((err = prepareRoot()) == updateos::NoError) {
            LOG_INFO("runUpdate %s root ready for reset", tarFileName.c_str());
        } else {
            LOG_ERROR("runUpdate %s can't prepare root dir for reset", tarFileName.c_str());
        }
    }
    else {
        LOG_ERROR("runUpdate %s open error %s", tarFileName.c_str(), mtar_strerror(ret));
        return (updateos::CantOpenUpdateFile);
    }

    return (updateos::NoError);
}

updateos::UpdateError UpdatePureOS::unpackUpdate()
{
    mtar_header_t h;
    filesInUpdatePackage.clear();
    while ((mtar_read_header(&updateTar, &h)) != MTAR_ENULLRECORD) {
        LOG_INFO("%s (%d bytes) type:%s\n", h.name, h.size, (h.type == MTAR_TDIR) ? "dir" : "file");
        unsigned long fileCRC32 = 0;
        if (h.type == MTAR_TDIR) {
            fs::path tmpPath = getUpdateTmpChild(h.name);
            if (ff_mkdir(tmpPath.c_str()) != 0) {
                LOG_ERROR("unpackAndMove failed to create %s when extracting update tar", tmpPath.c_str());
                return (updateos::CantCreateExtractedFile);
            }
        }
        else {
            if (unpackFileToTemp(h, &fileCRC32) == false) {
                LOG_ERROR("unpackAndMove failed to extract update file %s", h.name);
                return (updateos::CantCreateExtractedFile);
            }

            filesInUpdatePackage.push_back(FileInfo(h, fileCRC32));
        }
        mtar_next(&updateTar);
    }
    return (updateos::NoError);
}

updateos::UpdateError UpdatePureOS::verifyChecksums()
{
    std::unique_ptr<char[]> lineBuff(new char[purefs::buffer::tar_buf]);// max line should be freertos max path + checksum, so this is enough
    fs::path checksumsFile = getUpdateTmpChild(updateos::file::checksums);
    FF_FILE *fpChecksums = ff_fopen(checksumsFile.c_str(), "r");
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

                    getChecksumInfo(line,  filePath, &fileCRC32);
                    unsigned long computedCRC32 = getExtractedFileCRC32(filePath);
                    if (computedCRC32 != fileCRC32) {
                        LOG_ERROR("verifyChecksums %s crc32 match FAIL %lX != %lX", filePath.c_str(), fileCRC32, computedCRC32);
                        ff_fclose(fpChecksums);
                        return (updateos::VerifyChecksumsFailure);
                    }
                    else {
                        LOG_INFO("verifyChecksums %s crc32 match OK %lX == %lX", filePath.c_str(), fileCRC32, computedCRC32);
                    }
                }
            }
        }
        ff_fclose(fpChecksums);
    } else {
        LOG_ERROR("verifyChecksums can't open checksums file %s", checksumsFile.c_str());
        return (updateos::CantOpenChecksumsFile);
    }
    return (updateos::NoError);
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
        filePath  = infoLine.substr(0, lastSpacePos);
        const std::string fileCRC32Str = infoLine.substr(lastSpacePos + 1, purefs::buffer::crc_char_size-1);
        *fileCRC32Long = strtoull(fileCRC32Str.c_str(), nullptr, purefs::buffer::crc_radix);
        LOG_INFO("filePath: %s fileCRC32Str: %s fileCRC32Long: %lu", filePath.c_str(), fileCRC32Str.c_str(), *fileCRC32Long);
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
        LOG_ERROR("prepareRoot ff_deltree on %s caused an error %s", purefs::dir::os_previous.c_str(), strerror(stdioGET_ERRNO()));
    }

    if (isDirectory(purefs::dir::os_previous)) {
        LOG_ERROR("prepareRoot %s still exists, we can't continue", purefs::dir::os_previous.c_str());
        return (updateos::CantDeletePreviousOS);
    }
    // rename the current OS to previous on partition
    ret = ff_rename(purefs::dir::os_current.c_str(), purefs::dir::os_previous.c_str(), true);

    if (ret != 0) {
        LOG_ERROR("prepareRoot can't rename %s -> %s error %s", purefs::dir::os_current.c_str(), purefs::dir::os_previous.c_str(), strerror(stdioGET_ERRNO()));
        return (updateos::CantRenameCurrentToPrevious);
    }

    // rename the temp directory to current (extracted update)
    ret = ff_rename(updateTempDirectory.c_str(), purefs::dir::os_current.c_str(), true);

    if (ret != 0) {
        LOG_ERROR("prepareRoot can't rename %s -> %s error %s", updateTempDirectory.c_str(), purefs::dir::os_current.c_str(), strerror(stdioGET_ERRNO()));
        return (updateos::CantRenameTempToCurrent);
    }
    // make sure that boot.ini points to the current version

    return (updateBootINI());
}

updateos::UpdateError UpdatePureOS::updateBootINI()
{
    sbini_t *ini;
    ini = sbini_load(purefs::file::boot_ini.c_str());
    if (!ini){
        LOG_ERROR("updateBootINI can't open %s", purefs::file::boot_ini.c_str());
    }
    return (updateos::NoError);
}

bool UpdatePureOS::unpackFileToTemp(mtar_header_t &h, unsigned long *crc32)
{
    std::unique_ptr<unsigned char[]> readBuf(new unsigned char[purefs::buffer::tar_buf]);
    const fs::path fullPath = getUpdateTmpChild(h.name);
    LOG_INFO("unpackFileToTemp %s", fullPath.c_str());
    uint32_t blocksToRead = (h.size / purefs::buffer::tar_buf) + 1;
    uint32_t sizeToRead   = purefs::buffer::tar_buf;
    *crc32   = 0;
    int errCode = MTAR_ESUCCESS;
    FF_FILE *fp           = ff_fopen(fullPath.c_str(), "w+");
    if (fp == NULL) {
        LOG_INFO("unpackFileToTemp %s can't open for reading", fullPath.c_str());
        return (false);
    }

    for (uint32_t i = 0; i < blocksToRead; i++) {
        if (i + 1 == blocksToRead) {
            if (h.size < purefs::buffer::tar_buf)
                sizeToRead = h.size;
            else
                sizeToRead = updateTar.remaining_data;
        }

        if ((errCode = mtar_read_data(&updateTar, readBuf.get(), sizeToRead)) != MTAR_ESUCCESS)
        {
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

        *crc32 = Crc32_ComputeBuf(*crc32, readBuf.get(), sizeToRead);
    }
    LOG_INFO("unpackFileToTemp %s unpacked CRC32: %lX", fullPath.c_str(), *crc32);
    ff_fclose(fp);
    return (true);
}

void UpdatePureOS::cleanupAfterUpdate()
{
    LOG_INFO("cleanupAfterUpdate");
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
    updateTempDirectory = updateos::dir::tmp / generateRandomId(updateos::prefix_len);

    LOG_INFO("prepareTempDirForUpdate %s", updateTempDirectory.c_str());

    if (isDirectory(updateos::dir::updates) == false) {
        LOG_INFO("prepareTempDirForUpdate %s is not a directory", updateos::dir::updates.c_str());
        if (ff_mkdir(updateos::dir::updates.c_str()) != 0) {
            LOG_ERROR("%s can't create it %s", updateos::dir::updates.c_str(), strerror(stdioGET_ERRNO()));
            return (updateos::CantCreateUpdatesDir);
        }
        else {
            LOG_INFO("%s created", updateos::dir::updates.c_str());
        }
    }
    else {
        LOG_INFO("%s exists", updateos::dir::updates.c_str());
    }

    if (isDirectory(updateos::dir::tmp) == false) {
        LOG_INFO("prepareTempDirForUpdate %s is not a directory", updateos::dir::tmp.c_str());
        if (ff_mkdir(updateos::dir::tmp.c_str()) != 0) {
            LOG_ERROR("%s can't create it %s", updateos::dir::tmp.c_str(), strerror(stdioGET_ERRNO()));
            return (updateos::CantCreateTempDir);
        }
        else {
            LOG_INFO("prepareTempDirForUpdate %s created", updateos::dir::tmp.c_str());
        }
    }
    else {
        LOG_INFO("prepareTempDirForUpdate %s exists", updateos::dir::tmp.c_str());
    }

    if (isDirectory(updateTempDirectory)) {
        LOG_INFO("prepareTempDirForUpdate %s exists already, try to remove it", updateTempDirectory.c_str());
        if (ff_deltree(updateTempDirectory.c_str()) != 0) {
            LOG_ERROR("prepareTempDirForUpdate can't remove %s", updateTempDirectory.c_str());
            return (updateos::CantRemoveUniqueTmpDir);
        }
        else {
            LOG_INFO("prepareTempDirForUpdate %s removed", updateTempDirectory.c_str());
        }
    }

    LOG_INFO("prepareTempDirForUpdate trying to create %s as tempDir", updateTempDirectory.c_str());
    if (ff_mkdir(updateTempDirectory.c_str()) != 0) {
        LOG_ERROR("prepareTempDirForUpdate failed to create: %s error: %s",
                 updateTempDirectory.c_str(),
                 strerror(stdioGET_ERRNO()));
        return (updateos::CantCreateUniqueTmpDir);
    }

    LOG_INFO("prepareTempDirForUpdate tempDir selected %s", updateTempDirectory.c_str());
    return (updateos::NoError);
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
    FF_FindData_t *pxFindStruct;
    std::vector<FileInfo> updateFiles;
    pxFindStruct = (FF_FindData_t *)pvPortMalloc(sizeof(FF_FindData_t));
    memset(pxFindStruct, 0x00, sizeof(FF_FindData_t));

    if (ff_findfirst(updateos::dir::updates.c_str(), pxFindStruct) == 0) {
        do {
            /* Print the files name, size, and attribute string. */
            LOG_INFO("%s", pxFindStruct->pcFileName);
            if (endsWith(pxFindStruct->pcFileName, updateos::extension::update)) {
                updateFiles.push_back (pxFindStruct);
            }
        } while (ff_findnext(pxFindStruct) == 0);
    }

    vPortFree(pxFindStruct);
    return (json11::Json (updateFiles));
}