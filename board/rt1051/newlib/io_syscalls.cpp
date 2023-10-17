// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <newlib/vfs_io_syscalls.hpp>
#include <sys/statvfs.h>
#include <sys/mount.h>
#include <time/time_syscalls.hpp>

extern "C"
{
    using namespace vfsn::internal;

    /* File related functions */
    int _fstat_r(struct _reent *r, int fd, struct stat *st)
    {
        return syscalls::fstat(r->_errno, fd, st);
    }
    int _open_r(struct _reent *r, const char *file, int flags, int mode)
    {
        return syscalls::open(r->_errno, file, flags, mode);
    }
    long _close_r(struct _reent *r, int fd)
    {
        return syscalls::close(r->_errno, fd);
    }
    long _write_r(struct _reent *r, int fd, const void *buf, size_t cnt)
    {
        return syscalls::write(r->_errno, fd, buf, cnt);
    }
    long _read_r(struct _reent *r, int fd, void *buf, size_t cnt)
    {
        return syscalls::read(r->_errno, fd, buf, cnt);
    }
    off_t _lseek_r(struct _reent *r, int fd, off_t pos, int dir)
    {
        return syscalls::lseek(r->_errno, fd, pos, dir);
    }

    int _link_r(struct _reent *r, const char *existing, const char *newLink)
    {
        return syscalls::link(r->_errno, existing, newLink);
    }
    int _unlink_r(struct _reent *r, const char *name)
    {
        return syscalls::unlink(r->_errno, name);
    }
    int _fcntl_r(struct _reent *r, int fd, int cmd, int arg)
    {
        return syscalls::fcntl(r->_errno, fd, cmd, arg);
    }
    int _stat_r(struct _reent *r, const char *file, struct stat *pstat)
    {
        return syscalls::stat(r->_errno, file, pstat);
    }
    int rmdir(const char *path)
    {
        return syscalls::rmdir(_REENT->_errno, path);
    }
    /** POSIX directory related funcs */
    int chdir(const char *path)
    {
        return syscalls::chdir(_REENT->_errno, path);
    }
    char *getcwd(char *buf, size_t size)
    {
        return syscalls::getcwd(_REENT->_errno, buf, size);
    }
    int _rename_r(struct _reent *r, const char *oldName, const char *newName)
    {
        return syscalls::rename(r->_errno, oldName, newName);
    }
    int mkdir(const char *path, uint32_t mode)
    {
        return syscalls::mkdir(_REENT->_errno, path, mode);
    }
    // Dir entry functionality
    int closedir(DIR *dirp)
    {
        return syscalls::closedir(_REENT->_errno, dirp);
    }
    DIR *opendir(const char *dirname)
    {
        return syscalls::opendir(_REENT->_errno, dirname);
    }
    struct dirent *readdir(DIR *dirp)
    {
        return syscalls::readdir(_REENT->_errno, dirp);
    }
    int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
    {
        return syscalls::readdir_r(_REENT->_errno, dirp, entry, result);
    }
    void rewinddir(DIR *dirp)
    {
        syscalls::rewinddir(_REENT->_errno, dirp);
    }
    void seekdir(DIR *dirp, long int loc)
    {
        syscalls::seekdir(_REENT->_errno, dirp, loc);
    }
    long int telldir(DIR *dirp)
    {
        return syscalls::telldir(_REENT->_errno, dirp);
    }
    int chmod(const char *path, mode_t mode)
    {
        return syscalls::chmod(_REENT->_errno, path, mode);
    }
    int fchmod(int fd, mode_t mode)
    {
        return syscalls::fchmod(_REENT->_errno, fd, mode);
    }
    int fsync(int fd)
    {
        return syscalls::fsync(_REENT->_errno, fd);
    }
    int statvfs(const char *path, struct statvfs *buf)
    {
        return syscalls::statvfs(_REENT->_errno, path, buf);
    }
    /* Mount a filesystem.  */
    int mount(const char *special_file, const char *dir, const char *fstype, unsigned long int rwflag, const void *data)
    {
        return syscalls::mount(_REENT->_errno, special_file, dir, fstype, rwflag, data);
    }
    /* Unmount a filesystem.  */
    int umount(const char *special_file)
    {
        return syscalls::umount(_REENT->_errno, special_file);
    }
    int _gettimeofday_r(struct _reent *r, struct timeval *tp, void *tzp)
    {
        return utils::internal::syscalls::gettimeofday(r->_errno, tp, tzp);
    }
    void __tz_lock()
    {
        utils::internal::syscalls::tz_lock();
    }
    void __tz_unlock()
    {
        utils::internal::syscalls::tz_unlock();
    }
    void __env_lock(struct _reent *reent)
    {
        utils::internal::syscalls::env_lock(reent->_errno);
    }
    void __env_unlock(struct _reent *reent)
    {
        utils::internal::syscalls::env_unlock(reent->_errno);
    }
    ssize_t readlink(const char *path, char *buf, size_t buflen)
    {
        return syscalls::readlink(_REENT->_errno, path, buf, buflen);
    }
    int symlink(const char *name1, const char *name2)
    {
        return syscalls::symlink(_REENT->_errno, name1, name2);
    }
    long fpathconf(int fd, int name)
    {
        return syscalls::fpathconf(_REENT->_errno, fd, name);
    }
    long pathconf(const char *path, int name)
    {
        return syscalls::pathconf(_REENT->_errno, path, name);
    }
    int getentropy([[maybe_unused]] void *buf, [[maybe_unused]] size_t buflen)
    {
        errno = -ENOTSUP;
        return -1;
    }
}
