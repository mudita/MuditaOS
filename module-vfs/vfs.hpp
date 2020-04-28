/*
 *  @file vfs.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.04.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_VFS_HPP
#define PUREPHONE_VFS_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <sbini/sbini.h>
#include <crc32/crc32.h>
#include <log/log.hpp>

namespace fs = std::filesystem;

#ifdef TARGET_Linux
#include <cstdio>
#else
#include "ff_stdio.h"
#include "board/cross/eMMC/eMMC.hpp"
#endif

#define PATH_USER "/user/"
#define PATH_SYS  "/sys"

// this just concatenates two strings and creates a /user/ subdirectory filename
#define USER_PATH(file) PATH_USER file

namespace purefs
{
    namespace file
    {
        const inline fs::path boot_ini     = ".boot.ini";
        const inline fs::path boot_ini_bak = ".boot.ini.bak";
    }; // namespace file

    namespace dir
    {
        const inline fs::path eMMC_disk = PATH_SYS;
        const inline fs::path user_disk = PATH_USER;
    } // namespace dir

    namespace extension
    {
        const inline std::string crc32 = ".crc32";
    }

    namespace buffer
    {
        const inline int crc_buf       = 1024;
        const inline int crc_char_size = 9;
        const inline int crc_radix     = 16;
    } // namespace buffer

    namespace ini
    {
        const inline std::string main    = "main";
        const inline std::string os_type = "ostype";
    } // namespace ini
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
    /**
     * @brief Informs whether end of file was reached
     * @param stream to be checked.
     * @return true - file pointer is at the end of file, false otherwise.
     */
    bool eof(FILE *stream);

    /**
     * ext is an optional extension
     */
    std::vector<DirectoryEntry> listdir(const char *path, const std::string &ext = "");

    /**
     * @brief Reads line of text from opened file.
     * @note Returns string with line of text starting from the current file pointer position. Function ends with the /r
     * /n or 0 sign. Function checks if after /r there is /n if so it reads both signs.
     */
    std::string getline(FILE *stream, uint32_t length = 1024);

    FilesystemStats getFilesystemStats();


#ifndef TARGET_Linux
    bsp::eMMC emmc;
    FF_Disk_t *emmcFFDisk;
#endif
    std::string relativeToRoot(const std::string path);

    static unsigned long computeCRC32(FILE *file, unsigned long *outCrc32);
    static bool verifyCRC(const std::string filePath, const unsigned long crc32);
    static bool verifyCRC(const fs::path filePath);

  private:
    bool getOSRootFromIni();
    const fs::path getCurrentBootIni();
    fs::path osRootPath;
    std::string osType;
};

extern vfs vfs;

#endif // PUREPHONE_VFS_HPP
