// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <errno.h>
#include <dirent.h>
#include <log/log.hpp>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include "vfs_internal_dirent.hpp"
#include "HandleManager.hpp"
#include <newlib/vfs_io_syscalls.hpp>
#include <sys/statvfs.h>

namespace vfsn::internal::syscalls
{
    int open(int &_errno_, const char *file, int flags, int mode)
    {
        return -1;
    }

    long close(int &_errno_, int fd)
    {
        return -1;
    }

    long write(int &_errno_, int fd, const void *buf, size_t cnt)
    {
        return -1;
    }

    long read(int &_errno_, int fd, void *buf, size_t cnt)
    {
        return -1;
    }

    off_t lseek(int &_errno_, int fd, off_t pos, int dir)
    {
        return -1;
    }
    int fstat(int &_errno_, int fd, struct stat *pstat)
    {
        return -1;
    }
    int link(int &_errno_, const char *existing, const char *newLink)
    {
        return -1;
    }
    int unlink(int &_errno_, const char *name)
    {
        return -1;
    }

    int fcntl(int &_errno_, int fd, int cmd, int arg)
    {
        return -1;
    }
    int stat(int &_errno_, const char *file, struct stat *pstat)
    {
        return -1;
    }

    int chdir(int &_errno_, const char *path)
    {
        return -1;
    }
    char *getcwd(int &_errno_, char *buf, size_t size)
    {
        return nullptr;
    }
    int rename(int &_errno_, const char *oldName, const char *newName)
    {
        return -1;
    }

    int mkdir(int &_errno_, const char *path, uint32_t mode)
    {
        return -1;
    }

    DIR *opendir(int &_errno_, const char *dirname)
    {
        return nullptr;
    }

    int closedir(int &_errno_, DIR *dirp)
    {
        return -1;
    }

    struct dirent *readdir(int &_errno_, DIR *dirp)
    {
        return nullptr;
    }

    int readdir_r(int &_errno_, DIR *dirp, struct dirent *entry, struct dirent **result)
    {
        return -1;
    }

    void rewinddir(int &_errno_, DIR *dirp)
    {
    }

    void seekdir(int &_errno_, DIR *dirp, long int loc)
    {
    }
    long int telldir(int &_errno_, DIR *dirp)
    {
        return -1;
    }

    int chmod(int &_errno_, const char *path, mode_t mode)
    {
        return -1;
    }
    int fchmod(int &_errno_, int fd, mode_t mode)
    {
        return -1;
    }
    int fsync(int &_errno_, int fd)
    {
        return -1;
    }
    int statvfs(const char *path, struct statvfs *buf)
    {
        return -1;
    }

} // namespace vfsn::internal::syscalls

