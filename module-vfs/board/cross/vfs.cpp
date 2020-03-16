/*
 *  @file vfs.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.04.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "../../vfs.hpp"
#include "board/cross/eMMC/eMMC.hpp"
#include "ff_eMMC_user_disk.hpp"

#define eMMCHIDDEN_SECTOR_COUNT 8
#define eMMCPRIMARY_PARTITIONS  1
#define eMMCHUNDRED_64_BIT      100ULL
#define eMMCPARTITION_NUMBER    0 /* Only a single partition is used. */
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

    emmcFFDisk = FF_eMMC_user_DiskInit(eMMC_USER_DISK_NAME, &emmc);

    /* Print out information on the disk. */
    FF_eMMC_user_DiskShowPartition(emmcFFDisk);
}

vfs::FILE *vfs::fopen(const char *filename, const char *mode)
{
    return ff_fopen(filename, mode);
}

int vfs::fclose(FILE *stream)
{
    return ff_fclose(stream);
}

int vfs::remove(const char *name)
{
    return ff_remove(name);
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

std::vector<vfs::DirectoryEntry> vfs::listdir(const char *path, const std::string &ext)
{
    std::vector<DirectoryEntry> dir_list;

    FF_FindData_t *pxFindStruct;
    FileAttributes attribute;

    /* FF_FindData_t can be large, so it is best to allocate the structure
    dynamically, rather than declare it as a stack variable. */
    pxFindStruct = (FF_FindData_t *)malloc(sizeof(FF_FindData_t));

    /* FF_FindData_t must be cleared to 0. */
    memset(pxFindStruct, 0x00, sizeof(FF_FindData_t));

    /* The first parameter to ff_findfist() is the directory being searched.  Do
    not add wildcards to the end of the directory name. */
    if (ff_findfirst(path, pxFindStruct) == 0) {
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
            else if (pxFindStruct->ucAttributes & FF_FAT_ATTR_READONLY) {
                attribute = FileAttributes::ReadOnly;
            }
            else if ((pxFindStruct->ucAttributes & FF_FAT_ATTR_ARCHIVE) || (pxFindStruct->ucAttributes == 0)) {
                attribute = FileAttributes::Writable;
            }

            if (ext.empty()) {
                dir_list.push_back(DirectoryEntry{pxFindStruct->pcFileName, attribute, pxFindStruct->ulFileSize});
            }
            else {
                if (hasEnding(pxFindStruct->pcFileName, ext))
                    dir_list.push_back(DirectoryEntry{pxFindStruct->pcFileName, attribute, pxFindStruct->ulFileSize});
            }

        } while (ff_findnext(pxFindStruct) == 0);
    }

    /* Free the allocated FF_FindData_t structure. */
    free(pxFindStruct);

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
    char *buffer = (char *)malloc(length + 1);

    if (buffer == NULL)
        return std::string("");

    memset(buffer, 0, length + 1);

    uint32_t bytesRead = ff_fread(buffer, 1, length, stream);

    // search buffer for /n sign
    for (uint32_t i = 0; i < bytesRead; ++i) {
        if (buffer[i] == 0x0A) {
            buffer[i] = 0;
            ff_fseek(stream, currentPosition + i + 1, SEEK_SET);
            break;
        }
    }

    std::string ret = std::string(buffer);
    free(buffer);

    return ret;
}

vfs::FilesystemStats vfs::getFilesystemStats()
{
    FF_Error_t xError;
    uint64_t ullFreeSectors;
    uint32_t ulTotalSizeMB, ulFreeSizeMB;
    int iPercentageFree;
    FF_IOManager_t *pxIOManager;
    const char *pcTypeName = "unknown type";
    BaseType_t xReturn     = pdPASS;
    vfs::FilesystemStats filesystemStats;

    if (emmcFFDisk == NULL) {
        xReturn = pdFAIL;
    }
    else {
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

    return (filesystemStats);
}