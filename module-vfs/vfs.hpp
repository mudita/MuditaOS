#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <crc32/crc32.h>
#include <log/log.hpp>
#include <json/json11.hpp>

#ifdef TARGET_Linux
#include <cstdio>
#else
#include "ff_stdio.h"
#include "board/cross/eMMC/eMMC.hpp"
#endif

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
        const inline std::string crc32 = ".crc32";
    }

    namespace buffer
    {
        const inline int crc_buf       = 1024;
        const inline int crc_char_size = 9; // 9 ascii characters to store crc32 checksum
        const inline int crc_radix     = 16;
        const inline int tar_buf       = 8192 * 4;
        const inline int copy_buf      = 8192 * 4;
    } // namespace buffer

    namespace json
    {
        const inline std::string main       = "main";
        const inline std::string os_type    = "ostype";
        const inline std::string os_image   = "imagename";
        const inline std::string os_version = "version";
        const inline std::string timestamp  = "timestamp";

        const inline std::string git_info        = "git";
        const inline std::string os_git_tag      = "git_tag";
        const inline std::string os_git_revision = "git_commit";
        const inline std::string os_git_branch   = "git_branch";

        const inline std::string bootloader = "bootloader";
    } // namespace json

    struct boot_config_t
    {
        std::string os_image;
        std::string os_type;
        std::string os_version;
        std::string bootloader_verion;
        std::string timestamp;
        json11::Json boot_json_parsed;

        fs::path os_root_path;
        fs::path boot_json;

        void setVersion(const std::string versionString);
        [[nodiscard]] json11::Json to_json() const;
    };
};    // namespace purefs

class vfs
{
  public:
#ifdef TARGET_Linux
    using FILE = std::FILE;
#else
    using FILE = FF_FILE;
#endif

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

#ifndef TARGET_Linux
    bsp::eMMC emmc;
    FF_Disk_t *emmcFFDisk;
#endif

    static void computeCRC32(FILE *file, unsigned long *outCrc32);
    static bool verifyCRC(const std::string filePath, const unsigned long crc32);
    static bool verifyCRC(const fs::path filePath);
    static std::string generateRandomId(size_t length);

  private:
    bool updateFileCRC32(const fs::path &file);
    const fs::path getCurrentBootJSON();
    bool loadBootConfig(const fs::path &bootJsonPath);
    bool updateBootConfig(const fs::path &bootJsonPath);
    struct purefs::boot_config_t bootConfig;
};

extern vfs vfs;
