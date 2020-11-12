// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <ff_stdio.h>
#include <dirent.h>
#include <errno.h>
#include <cstring>
#include "dirent_support.hpp"
#include "debug.hpp"


struct __dirstream {
    long int        position;
    vfsn::linux::internal::diren::DIR_ITER*       dir_data;
    struct dirent   file_data;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"

extern "C" {
    using namespace vfsn::linux::internal;

    DIR *opendir(const char *dirname)
    {
        TRACE_SYSCALL();
		if(!dirname) {
			errno = EINVAL;
			return nullptr;
		}
        auto dir      = new DIR;
        dir->dir_data = diren::diropen(errno, dirname);
        if (!dir->dir_data) {
            delete dir;
            return nullptr;
        }
        dir->position            = 0;
        dir->file_data.d_ino     = -1;
        dir->file_data.d_name[0] = '\0';
        return dir;
    }
    __asm__(".symver diropen,diropen@GLIBC_2.2.5");

    int closedir(DIR *dirp)
    {
        TRACE_SYSCALL();
        if (!dirp) {
            errno = EBADF;
            return -1;
        }
        auto res = diren::dirclose(errno, dirp->dir_data);
        delete dirp;
        return res;
    }
    __asm__(".symver closedir,closedir@GLIBC_2.2.5");

    struct dirent *readdir(DIR *dirp)
    {
        TRACE_SYSCALL();
        if (!dirp) {
            errno = EBADF;
            return nullptr;
        }
        auto olderrno{errno};
        auto res = diren::dirnext(errno, dirp->dir_data);
        auto fff = reinterpret_cast<FF_FindData_t *>(dirp->dir_data->dir_state);
        if (res < 0) {
            if (errno == pdFREERTOS_ERRNO_ENMFILE) {
                errno = olderrno;
            }
            return nullptr;
        }
        dirp->position += 1;
        if (strnlen(fff->pcFileName, NAME_MAX) >= sizeof(dirp->file_data.d_name)) {
            errno = EOVERFLOW;
            return nullptr;
        }
        dirp->file_data.d_ino  = fff->xDirectoryEntry.ulObjectCluster;
        dirp->file_data.d_type = (fff->ucAttributes & FF_FAT_ATTR_DIR) ? DT_DIR : DT_REG;
        dirp->file_data.d_reclen = std::strlen( fff->pcFileName );
        std::strncpy(dirp->file_data.d_name, fff->pcFileName, sizeof(dirp->file_data.d_name));
        return &dirp->file_data;
    }
    __asm__(".symver readdir,readdir@GLIBC_2.2.5");

int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
    {
        TRACE_SYSCALL();
        if (!dirp) {
            errno = EBADF;
            return -1;
        }
        auto olderrno{errno};
        auto res = diren::dirnext(errno, dirp->dir_data);
        auto fff = reinterpret_cast<FF_FindData_t *>(dirp->dir_data->dir_state);
        if (res < 0) {
            res = errno;
            if (errno == pdFREERTOS_ERRNO_ENMFILE) {
                res = 0;
            }
            errno = olderrno;
            return res;
        }
        dirp->position += 1;
        if (strnlen(fff->pcFileName, NAME_MAX) >= sizeof(entry->d_name)) {
            return EOVERFLOW;
        }
        entry->d_ino  = fff->xDirectoryEntry.ulObjectCluster;
        entry->d_type = (fff->ucAttributes & FF_FAT_ATTR_DIR) ? DT_DIR : DT_REG;
        std::strncpy(entry->d_name, fff->pcFileName, sizeof(entry->d_name));
        *result = entry;
        return 0;
    }
    __asm__(".symver readdir_r,readdir_r@GLIBC_2.2.5");

    void rewinddir(DIR *dirp)
    {
        TRACE_SYSCALL();
        if (!dirp) {
            errno = EBADF;
            return;
        }
        diren::dirreset(errno, dirp->dir_data);
        dirp->position = 0;
    }
    __asm__(".symver rewinddir,rewinddir@GLIBC_2.2.5");

    void seekdir( DIR *dirp, long int loc)
    {
        TRACE_SYSCALL();
        if (!dirp) {
            errno = EBADF;
            return;
        }
        if (loc < 0) {
            return;
        }
        if (dirp->position > loc) {
            diren::dirreset(errno, dirp->dir_data);
            dirp->position = 0;
        }
        while ((dirp->position < loc) && (diren::dirnext(errno, dirp->dir_data) >= 0)) {
            dirp->position += 1;
        }
    }
    __asm__(".symver seekdir,seekdir@GLIBC_2.2.5");

    long int telldir(DIR *dirp)
    {
        TRACE_SYSCALL();
        if (!dirp) {
            errno = EBADF;
            return -1;
        }
        return dirp->position;
    }
    __asm__(".symver telldir,telldir@GLIBC_2.2.5");

}
#pragma GCC diagnostic pop
