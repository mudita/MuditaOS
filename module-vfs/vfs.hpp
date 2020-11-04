// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <crc32/crc32.h>
#include <log/log.hpp>
#include <json/json11.hpp>
#include "vfsNotifier.hpp"

#ifndef TARGET_Linux
#include "board/cross/eMMC/eMMC.hpp"
#endif

#include "ff_stdio.h"

#define PATH_SYS      "/sys"
#define PATH_USER     "user"
#define PATH_CURRENT  "current"
#define PATH_PREVIOUS "previous"
#define PATH_UPDATES  "updates"
#define PATH_TMP      "tmp"
#define PATH_BACKUP   "backup"
#define PATH_FACTORY  "factory"

// this just concatenates two strings and creates a /user/ subdirectory filename
#define USER_PATH(file) PATH_SYS "/" PATH_USER "/" file

namespace fs = std::filesystem;

namespace purefs
{
    namespace dir
    {
        const inline fs::path eMMC_disk   = PATH_SYS;
        const inline fs::path user_disk   = eMMC_disk / PATH_USER;
        const inline fs::path os_current  = eMMC_disk / PATH_CURRENT;
        const inline fs::path os_previous = eMMC_disk / PATH_PREVIOUS;
        const inline fs::path os_updates  = eMMC_disk / PATH_UPDATES;
        const inline fs::path tmp         = eMMC_disk / PATH_TMP;
        const inline fs::path os_backup   = eMMC_disk / PATH_BACKUP;
        const inline fs::path os_factory  = eMMC_disk / PATH_FACTORY;
    } // namespace dir

    namespace file
    {
        const inline fs::path boot_json = ".boot.json";
        const inline fs::path boot_bin  = "boot.bin";
    }; // namespace file

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
        inline constexpr auto version_inor    = "minor";
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

    struct BootConfig
    {
        std::string os_image;
        std::string os_type;
        std::string os_version;
        std::string bootloader_verion;
        std::string timestamp;
        json11::Json boot_json_parsed;

        fs::path os_root_path;
        fs::path boot_json;

        static int version_compare(const std::string &v1, const std::string &v2);
        [[nodiscard]] json11::Json to_json() const;
    };
}; // namespace purefs

class vfs
{
  public:
    using FILE = FF_FILE;
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
    FILE *fopen(const char *filename, const char *mode);
    int fclose(FILE *stream);
    int remove(const char *name);
    size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
    size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
    int fseek(FILE *stream, long int offset, int origin);
    long int ftell(FILE *stream);
    void rewind(FILE *stream);
    size_t filelength(FILE *stream);
    std::string getcurrdir();
    char *fgets(char *buffer, size_t count, FILE *stream);
    bool eof(FILE *stream);
    std::vector<DirectoryEntry> listdir(const char *path,
                                        const std::string &ext     = "",
                                        const bool bypassRootCheck = false);
    std::string getline(FILE *stream, uint32_t length = 1024);
    size_t fprintf(FILE *stream, const char *format, ...);
    FilesystemStats getFilesystemStats();
    std::string relativeToRoot(const std::string path);
    std::string lastErrnoToStr();
    bool isDir(const char *path);
    bool fileExists(const char *path);
    int deltree(const char *path);
    int mkdir(const char *dir);
    int rename(const char *oldname, const char *newname);
    std::string loadFileAsString(const fs::path &fileToLoad);
    bool replaceWithString(const fs::path &fileToModify, const std::string &stringToWrite);
    void updateTimestamp();
    struct purefs::BootConfig &getBootConfig()
    {
        return bootConfig;
    }
    void registerNotificationHandler(vfsn::utility::vfsNotifier::NotifyHandler handler)
    {
        chnNotifier.registerNotificationHandler(handler);
    }
    auto getAbsolutePath(std::string_view path) const -> std::string;

#ifndef TARGET_Linux
    bsp::eMMC emmc;
#endif

    FF_Disk_t *emmcFFDisk{};

    static void computeCRC32(FILE *file, unsigned long *outCrc32);
    static bool verifyCRC(const std::string filePath, const unsigned long crc32);
    static bool verifyCRC(const fs::path filePath);
    static std::string generateRandomId(size_t length);

  private:
    bool updateFileCRC32(const fs::path &file);
    const fs::path getCurrentBootJSON();
    bool loadBootConfig(const fs::path &bootJsonPath);
    bool updateBootConfig(const fs::path &bootJsonPath);
    struct purefs::BootConfig bootConfig;
    vfsn::utility::vfsNotifier chnNotifier;
};

extern vfs vfs;
