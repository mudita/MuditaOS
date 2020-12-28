// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "vfs.hpp"
#include <purefs/filesystem_paths.hpp>
#include <memory>
#include <cstring>
#include <log/log.hpp>

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

FILE *vfs::fopen(const char *filename, const char *mode)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return nullptr;
}

int vfs::fclose(FILE *stream)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return -1;
}

int vfs::remove(const char *name)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return -1;
}

size_t vfs::fread(void *ptr, size_t size, size_t count, FILE *stream)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return -1;
}

size_t vfs::fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return -1;
}

int vfs::fseek(FILE *stream, long int offset, int origin)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return -1;
}

long int vfs::ftell(FILE *stream)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return -1;
}

void vfs::rewind(FILE *stream)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
}

size_t vfs::filelength(FILE *stream)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return 0;
}

char *vfs::fgets(char *buff, size_t count, FILE *stream)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return nullptr;
}

std::string vfs::getcurrdir()
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return {};
}

static inline bool hasEnding(std::string const &fullString, std::string const &ending)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return false;
}

std::vector<vfs::DirectoryEntry> vfs::listdir(const char *path, const std::string &ext, const bool bypassRootCheck)
{

    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return {};
}

bool vfs::eof(FILE *stream)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return false;
}

std::string vfs::getline(FILE *stream, uint32_t length)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return {};
}

vfs::FilesystemStats vfs::getFilesystemStats()
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return {};
}

std::string vfs::relativeToRoot(const std::string path)
{
    return path;
}

bool vfs::isDir(const char *path)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return false;
}

bool vfs::fileExists(const char *path)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return false;
}

int vfs::deltree(const char *path)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return -1;
}

int vfs::mkdir(const char *dir)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return -1;
}

int vfs::rename(const char *oldname, const char *newname)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return -1;
}

std::string vfs::lastErrnoToStr()
{
    return {};
}

size_t vfs::fprintf(FILE *stream, const char *format, ...)
{
    LOG_FATAL("Unupported call [%s] !!!!", __PRETTY_FUNCTION__);
    return -1;
}
auto vfs::getAbsolutePath(std::string_view path) const -> std::string
{
    return std::string(path);
}

vfs::~vfs()
{}

vfs::vfs()
{}

void vfs::Init()
{}

#pragma GCC diagnostic pop
