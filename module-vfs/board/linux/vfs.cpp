/*
 *  @file vfs.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.04.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "../../vfs.hpp"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstdio>
#include <cstddef>

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "../module-utils/log/log.hpp"

namespace fs = std::filesystem;

vfs::vfs()
{}

vfs::~vfs()
{}

void vfs::Init()
{
    osRootPath = purefs::dir::eMMC_disk;
}

std::string vfs::relativeToRoot(const std::string path)
{
    return ((osRootPath / fs::path(path).relative_path()).relative_path());
}

FILE *vfs::fopen(const char *filename, const char *mode)
{
    return std::fopen(relativeToRoot(filename).c_str(), mode);
}

int vfs::fclose(FILE *stream)
{
    return std::fclose(stream);
}

int vfs::remove(const char *name)
{
    return std::remove(relativeToRoot(name).c_str());
}

size_t vfs::fread(void *ptr, size_t size, size_t count, FILE *stream)
{
    return std::fread(ptr, size, count, stream);
}

size_t vfs::fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
    return std::fwrite(ptr, size, count, stream);
}

int vfs::fseek(FILE *stream, long int offset, int origin)
{
    return std::fseek(stream, offset, origin);
}

long int vfs::ftell(FILE *stream)
{
    return std::ftell(stream);
}

void vfs::rewind(FILE *stream)
{
    std::rewind(stream);
}

bool vfs::eof(FILE *stream)
{
    return std::feof(stream);
}

size_t vfs::filelength(FILE *stream)
{

    size_t currPos = std::ftell(stream);

    std::fseek(stream, 0, SEEK_END);
    size_t size = std::ftell(stream);
    std::fseek(stream, currPos, SEEK_SET);

    return size;
}
char *vfs::fgets(char *buffer, size_t count, FILE *stream)
{
    return (fgets(buffer, count, stream));
}

std::string vfs::getcurrdir()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        return std::string{cwd};
    }
    else {
        return "";
    }
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
    FileAttributes attribute = FileAttributes::ReadOnly;
    size_t fileSize = 0;

    for (auto &p : fs::directory_iterator(relativeToRoot(path))) {
        if (fs::is_directory(p)) {
            attribute = FileAttributes ::Directory;
        }
        else if ((fs::status(p).permissions() & fs::perms::owner_write) != fs::perms::none) {
            attribute = FileAttributes ::Writable;
            fileSize  = std::filesystem::file_size(p);
        }
        else if ((fs::status(p).permissions() & fs::perms::owner_read) != fs::perms::none) {
            attribute = FileAttributes ::ReadOnly;
            fileSize  = std::filesystem::file_size(p);
        }

        auto pathStr = p.path().string();
        auto path    = pathStr.substr(pathStr.find_last_of("/\\") + 1);

        if (ext.empty()) {
            dir_list.push_back(DirectoryEntry{path, attribute, static_cast<uint32_t>(fileSize)});
        }
        else {
            if (hasEnding(path, ext))
                dir_list.push_back(DirectoryEntry{path, attribute, static_cast<uint32_t>(fileSize)});
        }
    }

    return dir_list;
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

    uint32_t bytesRead = fread(buffer, 1, length, stream);

    // search buffer for /n sign
    for (uint32_t i = 0; i < bytesRead; ++i) {
        if (buffer[i] == 0x0A) {
            buffer[i] = 0;
            fseek(stream, currentPosition + i + 1, SEEK_SET);
            break;
        }
    }

    std::string ret = std::string(buffer);
    free(buffer);

    return ret;
}

vfs::FilesystemStats vfs::getFilesystemStats()
{
    return (vfs::FilesystemStats());
}
