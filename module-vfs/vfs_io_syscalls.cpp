// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include "vfs_io_syscalls.hpp"
#include <errno.h>
#include <log/log.hpp>

struct stat;
namespace vfs::internal::syscalls
{
    int open(int &_errno_, const char *file, int flags, int mode)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    long close(int &_errno_, int fd)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    long write(int &_errno_, int fd, const void *buf, size_t cnt)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }

    long read(int &_errno_, int fd, void *buf, size_t cnt)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    off_t lseek(int &_errno_, int fd, off_t pos, int dir)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int fstat(int &_errno_, int fd, struct stat *st)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int link(int &_errno_, const char *existing, const char *newLink)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int unlink(int &_errno_, const char *name)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int fcntl(int &_errno_, int fd, int cmd, int arg)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int stat_r(int &_errno_, const char *file, struct stat *pstat)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int chdir(int &_errno_, const char *path)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    char *getcwd(int &_errno_, char *buf, size_t size)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return nullptr;
    }
    int rename(int &_errno_, const char *oldName, const char *newName)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int mkdir(int &_errno_, const char *path, uint32_t mode)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int closedir(int &_errno_, DIR *dirp)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    DIR *opendir(int &_errno_, const char *dirname)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return nullptr;
    }
    struct dirent *readdir(int &_errno_, DIR *dirp)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return nullptr;
    }
    int readdir_r(int &_errno_, DIR *dirp, struct dirent *entry, struct dirent **result)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    void rewinddir(int &_errno_, DIR *dirp)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
    }
    void seekdir(int &_errno_, DIR *dirp, long int loc)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
    }
    long int telldir(int &_errno_, DIR *dirp)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int chmod(int &_errno_, const char *path, mode_t mode)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int fchmod(int &_errno_, int fd, mode_t mode)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int fsync(int &_errno_, int fd)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
} // namespace vfs::io::syscalls