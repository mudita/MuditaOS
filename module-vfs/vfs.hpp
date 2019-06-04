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

#ifdef TARGET_Linux
#include <cstdio>
#else

#include "ff_stdio.h"
#include "eMMC/eMMC.hpp"
#endif

class vfs {

public:


#ifndef TARGET_Linux
    using FILE = FF_FILE;
#else
    using FILE = std::FILE;

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
        uint32_t    fileSize;
    };




    vfs();
    ~vfs();


    void Init();

    FILE * fopen ( const char * filename, const char * mode );

    int fclose ( FILE * stream );

    int remove (const char *name);

    size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );

    size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );

    int fseek ( FILE * stream, long int offset, int origin );

    long int ftell ( FILE * stream );

    void rewind ( FILE * stream );

    size_t filelength( FILE *stream );

    std::string getcurrdir();

    std::vector<DirectoryEntry> listdir(const char* path);

private:

    const char* eMMC_USER_DISK_NAME	= "/sys";

#ifndef TARGET_Linux
    bsp::eMMC emmc;
    FF_Disk_t* emmcFFDisk;
#endif
};


extern vfs vfs;

#endif //PUREPHONE_VFS_HPP
