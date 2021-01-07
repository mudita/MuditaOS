// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <newlib/vfs_io_syscalls.hpp>
#include <sys/statvfs.h>
extern "C"
{
    using namespace vfsn::internal;

    /* File related functions */
    int _fstat_r(struct _reent *r, int fd, struct stat *st)
    {
        return syscalls::fstat(r->_errno,fd,st);
    }
    int _open_r(struct _reent *r, const char *file, int flags, int mode)
    {
        return syscalls::open(r->_errno,file,flags,mode);
    }
    long _close_r(struct _reent *r, int fd)
    {
        return syscalls::close(r->_errno,fd);
    }
    long _write_r(struct _reent *r, int fd, const void *buf, size_t cnt)
    {
        return syscalls::write(r->_errno,fd,buf,cnt);
    }
    long _read_r(struct _reent *r, int fd, void *buf, size_t cnt)
    {
        return syscalls::read(r->_errno,fd,buf,cnt);
    }
    off_t _lseek_r(struct _reent *r, int fd, off_t pos, int dir)
    {
        return syscalls::lseek(r->_errno,fd,pos,dir);
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
        return syscalls::fcntl(r->_errno,fd,cmd,arg);
    }
    int _stat_r(struct _reent *r, const char *file, struct stat *pstat)
    {
        return syscalls::stat(r->_errno,file,pstat);
    }

    /** POSIX directory related funcs */
    int chdir(const char *path)
    {
        return syscalls::chdir(_REENT->_errno,path);
    }
    char *getcwd(char *buf, size_t size)
    {
        return syscalls::getcwd(_REENT->_errno,buf,size);
    }
    int _rename_r(struct _reent *r, const char *oldName, const char *newName)
    {
        return syscalls::rename(r->_errno,oldName,newName);
    }
    int mkdir(const char *path, uint32_t mode)
    {
        return syscalls::mkdir(_REENT->_errno,path,mode);
    }
    // Dir entry functionality
    int closedir(DIR *dirp)
    {
        return syscalls::closedir(_REENT->_errno,dirp);
    }
    DIR *opendir(const char *dirname)
    {
        return syscalls::opendir(_REENT->_errno, dirname);
    }
    struct dirent *readdir(DIR *dirp) {
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
        syscalls::seekdir(_REENT->_errno, dirp,loc);
    }
    long int telldir(DIR *dirp)
    {
        return syscalls::telldir(_REENT->_errno, dirp);
    }
    int chmod(const char* path, mode_t mode) {
        return syscalls::chmod(_REENT->_errno,path,mode);
    }
    int fchmod(int fd, mode_t mode)
    {
        return syscalls::fchmod(_REENT->_errno, fd, mode);
    }
    int fsync( int fd ) {
        return syscalls::fsync(_REENT->_errno, fd);
    }
    int statvfs(const char *path, struct statvfs *buf)
    {
        return syscalls::statvfs(path,buf);
    }
}

