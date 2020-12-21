// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "vfs.hpp"
#include <purefs/filesystem_paths.hpp>
#include <memory>
#include <cstring>

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

std::atomic<bool> vfs::initDone{false};

vfs::FILE *vfs::fopen(const char *filename, const char *mode)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return nullptr;
    }
    const auto filename_rel = relativeToRoot(filename);
    const auto handle       = ff_fopen(filename_rel.c_str(), mode);
    chnNotifier.onFileOpen(filename_rel, mode, handle);
    return handle;
}

int vfs::fclose(FILE *stream)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return -1;
    }
    const auto ret = ff_fclose(stream);
    chnNotifier.onFileClose(stream);
    return ret;
}

int vfs::remove(const char *name)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return -1;
    }
    const auto abs_name = relativeToRoot(name);
    const auto ret      = ff_remove(abs_name.c_str());
    if (!ret)
        chnNotifier.onFileRemove(abs_name);
    return ret;
}

size_t vfs::fread(void *ptr, size_t size, size_t count, FILE *stream)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return ssize_t(-1);
    }
    return ff_fread(ptr, size, count, stream);
}

size_t vfs::fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return size_t(0);
    }
    return ff_fwrite(ptr, size, count, stream);
}

int vfs::fseek(FILE *stream, long int offset, int origin)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return -1;
    }
    return ff_fseek(stream, offset, origin);
}

long int vfs::ftell(FILE *stream)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return -1;
    }
    return ff_ftell(stream);
}

void vfs::rewind(FILE *stream)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
    }
    ff_rewind(stream);
}

size_t vfs::filelength(FILE *stream)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return 0;
    }
    return ff_filelength(stream);
}

char *vfs::fgets(char *buff, size_t count, FILE *stream)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return nullptr;
    }
    return ff_fgets(buff, count, stream);
}

std::string vfs::getcurrdir()
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return {};
    }
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
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return {};
    }
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
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return true;
    }
    return ff_feof(stream);
}

std::string vfs::getline(FILE *stream, uint32_t length)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return {};
    }
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

    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return {};
    }
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
        if (bootConfig.os_root_path().root_directory() == fsPath.root_directory())
            return fsPath;
        else
            return purefs::createPath(purefs::dir::getRootDiskPath(), fsPath.relative_path()).c_str();
    }

    if (path.empty())
        return bootConfig.os_root_path();
    else
        return bootConfig.os_root_path() / fsPath;
}

bool vfs::isDir(const char *path)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return false;
    }
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

    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return false;
    }
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
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return -1;
    }
    if (path != nullptr)
        return ff_deltree(
            relativeToRoot(path).c_str(),
            [](void *ctx, const char *path) {
                auto _this = reinterpret_cast<vfs *>(ctx);
                _this->chnNotifier.onFileRemove(path);
            },
            this);
    else
        return -1;
}

int vfs::mkdir(const char *dir)
{
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return -1;
    }
    if (dir != nullptr)
        return ff_mkdir(relativeToRoot(dir).c_str());
    else
        return -1;
}

int vfs::rename(const char *oldname, const char *newname)
{

    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return -1;
    }
    if (oldname != nullptr && newname != nullptr) {
        const auto old_rel = relativeToRoot(oldname);
        const auto new_rel = relativeToRoot(newname);
        const auto ret     = ff_rename(old_rel.c_str(), new_rel.c_str(), true);
        if (!ret)
            chnNotifier.onFileRename(new_rel, old_rel);
        return ret;
    }
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

    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return 0;
    }
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
auto vfs::getAbsolutePath(std::string_view path) const -> std::string
{
    namespace fs = std::filesystem;
    if (!initDone) {
        stdioSET_ERRNO(pdFREERTOS_ERRNO_EIO);
        return {};
    }
    fs::path fs_path(path);
    if (fs_path.is_relative()) {
        char cwd_path[ffconfigMAX_FILENAME];
        ff_getcwd(cwd_path, sizeof cwd_path);
        fs::path fs_cwd(cwd_path);
        return fs_cwd / fs_path;
    }
    else {
        return std::string(path);
    }
}

#pragma GCC diagnostic pop
