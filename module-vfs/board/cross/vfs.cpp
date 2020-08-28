/*
 *  @file vfs.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.04.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "../../vfs.hpp"
#include "ff_eMMC_user_disk.hpp"

#include <memory>

#define eMMCHIDDEN_SECTOR_COUNT 8
#define eMMCPRIMARY_PARTITIONS  2
#define eMMCHUNDRED_64_BIT      100ULL
#define eMMCPARTITION_NUMBER    0
#define eMMCBYTES_PER_MB        (1024ull * 1024ull)
#define eMMCSECTORS_PER_MB      (eMMCBYTES_PER_MB / 512ull)

/* Used as a magic number to indicate that an FF_Disk_t structure is a RAM
disk. */
#define eMMCSIGNATURE             0x61606362
#define mainIO_MANAGER_CACHE_SIZE (15UL * FSL_SDMMC_DEFAULT_BLOCK_SIZE)

vfs::vfs() : emmc()
{}

vfs::~vfs()
{
    FF_eMMC_user_DiskDelete(emmcFFDisk);
    emmc.DeInit();
}

void vfs::Init()
{
    emmc.Init();

    emmcFFDisk = FF_eMMC_user_DiskInit(purefs::dir::eMMC_disk.c_str(), &emmc);

    /* Print out information on the disk. */
    FF_eMMC_user_DiskShowPartition(emmcFFDisk);

    bootConfig.os_root_path = purefs::dir::eMMC_disk;

    if (loadBootConfig(getCurrentBootJSON())) {
        LOG_INFO("vfs::Init osType %s root:%s", bootConfig.os_type.c_str(), bootConfig.os_root_path.c_str());
        if (ff_chdir(bootConfig.os_root_path.c_str()) != 0) {
            LOG_ERROR("vfs::Init can't chdir to %s", bootConfig.os_root_path.c_str());
        }
    }
    else {
        LOG_ERROR("vfs::Init unable to determine OS type, fallback to %s", bootConfig.os_root_path.c_str());
    }

    LOG_INFO("vfs::Init running on ARM osRootPath: %s", bootConfig.os_root_path.c_str());

    // this should already exist and have user data in it
    // if it does not create an exmpty directory so that sqlite3 does not fault
    if (isDir(purefs::dir::user_disk.c_str()) == false) {
        LOG_ERROR("vfs::Init looks like %s does not exist, try to create it", purefs::dir::user_disk.c_str());
        if (ff_mkdir(purefs::dir::user_disk.c_str()) != 0) {
            LOG_ERROR("vfs::Init can't create %s directory", purefs::dir::user_disk.c_str());
        }
    }
    else {
        LOG_INFO("vfs::Init looks like %s exists", purefs::dir::user_disk.c_str());
    }
}

vfs::FILE *vfs::fopen(const char *filename, const char *mode)
{
    return ff_fopen(relativeToRoot(filename).c_str(), mode);
}

int vfs::fclose(FILE *stream)
{
    return ff_fclose(stream);
}

int vfs::remove(const char *name)
{
    return ff_remove(relativeToRoot(name).c_str());
}

size_t vfs::fread(void *ptr, size_t size, size_t count, FILE *stream)
{
    return ff_fread(ptr, size, count, stream);
}

size_t vfs::fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
    return ff_fwrite(ptr, size, count, stream);
}

int vfs::fseek(FILE *stream, long int offset, int origin)
{
    return ff_fseek(stream, offset, origin);
}

long int vfs::ftell(FILE *stream)
{
    return ff_ftell(stream);
}

void vfs::rewind(FILE *stream)
{
    ff_rewind(stream);
}

size_t vfs::filelength(FILE *stream)
{
    return ff_filelength(stream);
}

char *vfs::fgets(char *buff, size_t count, FILE *stream)
{
    return ff_fgets(buff, count, stream);
}

std::string vfs::getcurrdir()
{
    char buff[64] = {};
    ff_getcwd(buff, sizeof buff);
    return std::string{buff};
}

static inline bool hasEnding(std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

std::vector<vfs::DirectoryEntry> vfs::listdir(const char *path, const std::string &ext, const bool bypassRootCheck)
{
    std::vector<DirectoryEntry> dir_list;

    FileAttributes attribute;

    /* FF_FindData_t can be large, so it is best to allocate the structure
    dynamically, rather than declare it as a stack variable. */
    auto pxFindStruct = std::make_unique<FF_FindData_t>();

    /* FF_FindData_t must be cleared to 0. */
    memset(pxFindStruct.get(), 0x00, sizeof(FF_FindData_t));

    /* The first parameter to ff_findfist() is the directory being searched.  Do
    not add wildcards to the end of the directory name. */
    if (ff_findfirst(bypassRootCheck ? path : relativeToRoot(path).c_str(), pxFindStruct.get()) == 0 &&
        pxFindStruct != nullptr) {
        do {
            if ((pxFindStruct->ucAttributes & FF_FAT_ATTR_HIDDEN) ||
                (pxFindStruct->ucAttributes & FF_FAT_ATTR_SYSTEM) || (pxFindStruct->ucAttributes & FF_FAT_ATTR_VOLID) ||
                (pxFindStruct->ucAttributes & FF_FAT_ATTR_LFN)) {
                continue;
            }
            /* Point pcAttrib to a string that describes the file. */
            else if ((pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR) != 0) {
                attribute = FileAttributes::Directory;
            }
            else if ((pxFindStruct->ucAttributes & FF_FAT_ATTR_ARCHIVE) || (pxFindStruct->ucAttributes == 0)) {
                attribute = FileAttributes::Writable;
            }
            else {
                attribute = FileAttributes::ReadOnly;
            }

            if (ext.empty()) {
                dir_list.push_back(DirectoryEntry{pxFindStruct->pcFileName, attribute, pxFindStruct->ulFileSize});
            }
            else {
                if (hasEnding(pxFindStruct->pcFileName, ext))
                    dir_list.push_back(DirectoryEntry{pxFindStruct->pcFileName, attribute, pxFindStruct->ulFileSize});
            }

        } while (ff_findnext(pxFindStruct.get()) == 0);
    }

    return dir_list;
}

bool vfs::eof(FILE *stream)
{
    return ff_feof(stream);
}

std::string vfs::getline(FILE *stream, uint32_t length)
{

    uint32_t currentPosition = ftell(stream);

    // allocate memory to read number of signs defined by length param. Size of buffer is increased by 1 to add string's
    // null terminator.
    std::unique_ptr<char[]> buffer(new char[length + 1]);
    memset(buffer.get(), 0, length + 1);

    uint32_t bytesRead = ff_fread(buffer.get(), 1, length, stream);

    // search buffer for /n sign
    for (uint32_t i = 0; i < bytesRead; ++i) {
        if (buffer[i] == 0x0A) {
            buffer[i] = 0;
            ff_fseek(stream, currentPosition + i + 1, SEEK_SET);
            break;
        }
    }

    std::string ret = std::string(buffer.get());

    return ret;
}

vfs::FilesystemStats vfs::getFilesystemStats()
{
    FF_Error_t xError;
    uint64_t ullFreeSectors;
    uint32_t ulTotalSizeMB, ulFreeSizeMB;
    int iPercentageFree;
    FF_IOManager_t *pxIOManager;
    vfs::FilesystemStats filesystemStats;

    if (emmcFFDisk != NULL) {
        pxIOManager = emmcFFDisk->pxIOManager;

        switch (pxIOManager->xPartition.ucType) {
        case FF_T_FAT12:
            filesystemStats.type = "FAT12";
            break;

        case FF_T_FAT16:
            filesystemStats.type = "FAT16";
            break;

        case FF_T_FAT32:
            filesystemStats.type = "FAT32";
            break;

        default:
            filesystemStats.type = "UNKOWN";
            break;
        }

        FF_GetFreeSize(pxIOManager, &xError);

        ullFreeSectors  = pxIOManager->xPartition.ulFreeClusterCount * pxIOManager->xPartition.ulSectorsPerCluster;
        iPercentageFree = (int)((eMMCHUNDRED_64_BIT * ullFreeSectors + pxIOManager->xPartition.ulDataSectors / 2) /
                                ((uint64_t)pxIOManager->xPartition.ulDataSectors));

        ulTotalSizeMB = pxIOManager->xPartition.ulDataSectors / eMMCSECTORS_PER_MB;
        ulFreeSizeMB  = (uint32_t)(ullFreeSectors / eMMCSECTORS_PER_MB);

        filesystemStats.freeMbytes  = ulFreeSizeMB;
        filesystemStats.totalMbytes = ulTotalSizeMB;
        filesystemStats.freePercent = iPercentageFree;
    }

    return filesystemStats;
}

std::string vfs::relativeToRoot(const std::string path)
{
    fs::path fsPath(path);

    if (fsPath.is_absolute()) {
        if (bootConfig.os_root_path.root_directory() == fsPath.root_directory())
            return fsPath;
        else
            return (purefs::dir::eMMC_disk / fsPath.relative_path()).c_str();
    }

    if (path.empty())
        return bootConfig.os_root_path;
    else
        return bootConfig.os_root_path / fsPath;
}

bool vfs::isDir(const char *path)
{
    if (path == nullptr)
        return false;

    FF_Stat_t fileStatus;

    const int ret = ff_stat(relativeToRoot(path).c_str(), &fileStatus);
    if (ret == 0) {
        return (fileStatus.st_mode == FF_IFDIR);
    }
    else {
        return false;
    }
}

bool vfs::fileExists(const char *path)
{
    if (path == nullptr)
        return false;

    FF_Stat_t fileStatus;
    const int ret = ff_stat(relativeToRoot(path).c_str(), &fileStatus);
    if (ret == 0) {
        return true;
    }
    return false;
}

int vfs::deltree(const char *path)
{
    if (path != nullptr)
        return ff_deltree(relativeToRoot(path).c_str());
    else
        return -1;
}

int vfs::mkdir(const char *dir)
{
    if (dir != nullptr)
        return ff_mkdir(relativeToRoot(dir).c_str());
    else
        return -1;
}

int vfs::rename(const char *oldname, const char *newname)
{
    if (oldname != nullptr && newname != nullptr)
        return ff_rename(relativeToRoot(oldname).c_str(), relativeToRoot(newname).c_str(), true);
    else
        return -1;
}

std::string vfs::lastErrnoToStr()
{
    return (strerror(stdioGET_ERRNO()));
}

size_t vfs::fprintf(FILE *stream, const char *format, ...)
{
    size_t count;
    size_t result;
    char *buffer = nullptr;
    va_list args;

    buffer = static_cast<char *>(ffconfigMALLOC(ffconfigFPRINTF_BUFFER_LENGTH));
    if (buffer == nullptr) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_ENOMEM);
        return -1;
    }

    va_start(args, format);
    count = vsnprintf(buffer, ffconfigFPRINTF_BUFFER_LENGTH, format, args);
    va_end(args);

    if (count > 0) {
        result = ff_fwrite(buffer, 1, count, stream);
        if (result < count) {
            count = -1;
        }
    }

    ffconfigFREE(buffer);
    return count;
}
