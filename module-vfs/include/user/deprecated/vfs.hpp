// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <log/log.hpp>
#include <atomic>
#include <boot/bootconfig.hpp>
#include "vfsNotifier.hpp"
#include "vfs_globals.hpp"

#ifndef TARGET_Linux
#include "board/cross/eMMC/eMMC.hpp"
#endif

#include "ff_stdio.h"

namespace fs = std::filesystem;

namespace purefs
{
    namespace extension
    {
        inline constexpr auto crc32 = ".crc32";
    }

    namespace buffer
    {
        inline constexpr auto crc_buf       = 1024;
        inline constexpr auto crc_char_size = 9;
        inline constexpr auto crc_radix     = 16;
        inline constexpr auto tar_buf       = 8192 * 4;
        inline constexpr auto copy_buf      = 8192 * 4;
    } // namespace buffer

    namespace json
    {
        inline constexpr auto main            = "main";
        inline constexpr auto os_type         = "ostype";
        inline constexpr auto os_image        = "imagename";
        inline constexpr auto os_version      = "version";
        inline constexpr auto version_major   = "major";
        inline constexpr auto version_minor   = "minor";
        inline constexpr auto version_patch   = "patch";
        inline constexpr auto version_string  = "string";
        inline constexpr auto timestamp       = "timestamp";
        inline constexpr auto misc            = "misc";
        inline constexpr auto builddate       = "builddate";
        inline constexpr auto git_info        = "git";
        inline constexpr auto os_git_tag      = "git_tag";
        inline constexpr auto os_git_revision = "git_commit";
        inline constexpr auto os_git_branch   = "git_branch";
        inline constexpr auto bootloader      = "bootloader";
    } // namespace json

}; // namespace purefs

/* NOTE: VFS global object class is now deprecated more information
 * on the confuence page:
 * https://appnroll.atlassian.net/wiki/spaces/MFP/pages/706248802/VFS+library+migration+guide
 */
class vfs
{
  public:
    using FILE    = FF_FILE;
    using FsEvent = vfsn::utility::vfsNotifier::FsEvent;
    enum class FileAttributes
    {
        ReadOnly,
        Writable,
        Directory
    };

    struct DirectoryEntry
    {
        std::string fileName;
        FileAttributes attributes;
        uint32_t fileSize;
        json11::Json to_json() const
        {
            return (json11::Json::object{{"name", fileName}, {"size", std::to_string(fileSize)}});
        }
    };

    struct FilesystemStats
    {
        std::string type;
        uint32_t freeMbytes;
        uint32_t freePercent;
        uint32_t totalMbytes;
    };
    vfs();
    ~vfs();
    void Init();
    [[deprecated]] FILE *fopen(const char *filename, const char *mode);
    [[deprecated]] int fclose(FILE *stream);
    [[deprecated]] int remove(const char *name);
    [[deprecated]] size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
    [[deprecated]] size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
    [[deprecated]] int fseek(FILE *stream, long int offset, int origin);
    [[deprecated]] long int ftell(FILE *stream);
    [[deprecated]] void rewind(FILE *stream);
    [[deprecated]] size_t filelength(FILE *stream);
    [[deprecated]] std::string getcurrdir();
    [[deprecated]] char *fgets(char *buffer, size_t count, FILE *stream);
    [[deprecated]] bool eof(FILE *stream);
    [[deprecated]] std::vector<DirectoryEntry> listdir(const char *path,
                                                       const std::string &ext     = "",
                                                       const bool bypassRootCheck = false);
    [[deprecated]] std::string getline(FILE *stream, uint32_t length = 1024);
    [[deprecated]] size_t fprintf(FILE *stream, const char *format, ...);
    [[deprecated]] FilesystemStats getFilesystemStats();
    [[deprecated]] std::string relativeToRoot(const std::string path);
    [[deprecated]] std::string lastErrnoToStr();
    [[deprecated]] bool isDir(const char *path);
    [[deprecated]] bool fileExists(const char *path);
    [[deprecated]] int deltree(const char *path);
    [[deprecated]] int mkdir(const char *dir);
    [[deprecated]] int rename(const char *oldname, const char *newname);
    [[deprecated]] std::string loadFileAsString(const fs::path &fileToLoad);
    [[deprecated]] bool replaceWithString(const fs::path &fileToModify, const std::string &stringToWrite);
    [[deprecated]] void updateTimestamp();
    [[deprecated]] void registerNotificationHandler(vfsn::utility::vfsNotifier::NotifyHandler handler)
    {
        chnNotifier.registerNotificationHandler(handler);
    }
    [[deprecated]] auto getAbsolutePath(std::string_view path) const -> std::string;

#ifndef TARGET_Linux
    bsp::eMMC emmc;
#endif

    FF_Disk_t *emmcFFDisk{};

    auto isInitialized() const noexcept
    {
        return initDone.load();
    }

  private:
    vfsn::utility::vfsNotifier chnNotifier;
    static std::atomic<bool> initDone;
    boot::BootConfig bootConfig;
};

extern vfs vfs;
