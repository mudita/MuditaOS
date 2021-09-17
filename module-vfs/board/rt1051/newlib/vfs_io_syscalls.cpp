// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/statvfs.h>
#include <purefs/vfs_subsystem.hpp>
#include <newlib/vfs_io_syscalls.hpp>

struct __dirstream
{
    purefs::fs::filesystem::fsdir dirh;
    size_t position;
    struct dirent dir_data;
};

namespace
{
    template <class Base, typename T, typename... Args>
    auto invoke_fs(int &error, T Base::*lfs_fun, Args &&... args)
        -> decltype((static_cast<Base *>(nullptr)->*lfs_fun)(std::forward<Args>(args)...))
    {
        auto vfs = purefs::subsystem::vfs_core();
        if (!vfs) {
            error = EIO;
            return -1;
        }
        auto ret = (vfs.get()->*lfs_fun)(std::forward<Args>(args)...);
        if (ret < 0) {
            error = -ret;
            ret   = -1;
        }
        return ret;
    }
    auto stmode_to_type(mode_t mode)
    {
        if (S_ISREG(mode)) {
            return DT_REG;
        }
        else if (S_ISDIR(mode)) {
            return DT_DIR;
        }
        else if (S_ISCHR(mode)) {
            return DT_CHR;
        }
        else if (S_ISBLK(mode)) {
            return DT_BLK;
        }
        else if (S_ISFIFO(mode)) {
            return DT_FIFO;
        }
        else if (S_ISSOCK(mode)) {
            return DT_SOCK;
        }
        else {
            return 0;
        }
    }
} // namespace

namespace vfsn::internal::syscalls
{
    int open(int &_errno_, const char *file, int flags, int mode)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::open, file, flags, mode);
    }

    long close(int &_errno_, int fd)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::close, fd);
    }

    long write(int &_errno_, int fd, const void *buf, size_t cnt)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::write, fd, reinterpret_cast<const char *>(buf), cnt);
    }

    long read(int &_errno_, int fd, void *buf, size_t cnt)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::read, fd, reinterpret_cast<char *>(buf), cnt);
    }

    off_t lseek(int &_errno_, int fd, off_t pos, int dir)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::seek, fd, pos, dir);
    }

    int fstat(int &_errno_, int fd, struct stat *pstat)
    {
        if (pstat) {
            return invoke_fs(_errno_, &purefs::fs::filesystem::fstat, fd, *pstat);
        }
        else {
            _errno_ = EINVAL;
            return -1;
        }
    }

    int link(int &_errno_, const char *existing, const char *newLink)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::link, existing, newLink);
    }

    int unlink(int &_errno_, const char *name)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::unlink, name);
    }

    int fcntl(int &_errno_, int fd, int cmd, int arg)
    {
        _errno_ = ENOTSUP;
        return -1;
    }

    int stat(int &_errno_, const char *file, struct stat *pstat)
    {
        if (pstat) {
            return invoke_fs(_errno_, &purefs::fs::filesystem::stat, file, *pstat);
        }
        else {
            _errno_ = EINVAL;
            return -1;
        }
    }

    int chdir(int &_errno_, const char *path)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::chdir, path);
    }

    char *getcwd(int &_errno_, char *buf, size_t size)
    {
        if (!buf) {
            _errno_ = EINVAL;
            return nullptr;
        }
        auto vfs = purefs::subsystem::vfs_core();
        if (!vfs) {
            _errno_ = EIO;
            return nullptr;
        }
        const auto ret  = vfs->getcwd();
        const auto slen = ret.copy(buf, size);
        if (size >= slen) {
            buf[slen] = '\0';
        }
        return buf;
    }

    int rename(int &_errno_, const char *oldName, const char *newName)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::rename, oldName, newName);
    }

    int mkdir(int &_errno_, const char *path, uint32_t mode)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::mkdir, path, mode);
    }

    DIR *opendir(int &_errno_, const char *dirname)
    {
        __dirstream *ret{};
        if (!dirname) {
            _errno_ = EIO;
            return ret;
        }
        auto vfs = purefs::subsystem::vfs_core();
        if (!vfs) {
            _errno_ = EIO;
            return ret;
        }
        ret = new (std::nothrow) __dirstream;
        if (!ret) {
            _errno_ = ENOMEM;
            return ret;
        }
        ret->position = 0;
        ret->dirh     = vfs->diropen(dirname);
        if (!ret->dirh) {
            delete ret;
            _errno_ = EIO;
            ret     = nullptr;
        }
        else if (ret->dirh->error()) {
            _errno_ = -ret->dirh->error();
            delete ret;
            ret = nullptr;
        }
        return ret;
    }

    int closedir(int &_errno_, DIR *dirp)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return -1;
        }
        auto vfs = purefs::subsystem::vfs_core();
        if (!vfs) {
            _errno_ = EIO;
            return -1;
        }
        auto ret = vfs->dirclose(dirp->dirh);
        if (ret < 0) {
            _errno_ = -ret;
            ret     = -1;
        }
        delete dirp;
        return ret;
    }

    struct dirent *readdir(int &_errno_, DIR *dirp)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return nullptr;
        }
        auto vfs = purefs::subsystem::vfs_core();
        if (!vfs) {
            _errno_ = EIO;
            return nullptr;
        }
        std::string fname;
        struct stat stdata;
        auto ret = vfs->dirnext(dirp->dirh, fname, stdata);
        if (ret < 0) {
            if (ret != -ENODATA) {
                _errno_ = -ret;
            }
            return nullptr;
        }
        else {
            if (fname.size() >= sizeof(dirp->dir_data.d_name)) {
                _errno_ = EOVERFLOW;
                return nullptr;
            }
            dirp->position += 1;
            dirp->dir_data.d_ino    = stdata.st_ino;
            dirp->dir_data.d_type   = stmode_to_type(stdata.st_mode);
            dirp->dir_data.d_reclen = fname.size();
            std::strncpy(dirp->dir_data.d_name, fname.c_str(), sizeof(dirp->dir_data.d_name));
            return &dirp->dir_data;
        }
    }

    int readdir_r(int &_errno_, DIR *dirp, struct dirent *entry, struct dirent **result)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return -1;
        }
        if (!*result) {
            _errno_ = EINVAL;
            return -1;
        }
        auto vfs = purefs::subsystem::vfs_core();
        if (!vfs) {
            _errno_ = EIO;
            return -1;
        }
        std::string fname;
        struct stat stdata;
        auto ret = vfs->dirnext(dirp->dirh, fname, stdata);
        if (ret == -ENODATA) {
            *result = nullptr;
            return 0;
        }
        else if (ret < 0) {
            _errno_ = -ret;
            return -1;
        }
        else {
            if (fname.size() >= sizeof(dirp->dir_data.d_name)) {
                _errno_ = EOVERFLOW;
                return -1;
            }
            dirp->position += 1;
            entry->d_ino    = stdata.st_ino;
            entry->d_type   = stmode_to_type(stdata.st_mode);
            entry->d_reclen = fname.size();
            std::strncpy(entry->d_name, fname.c_str(), sizeof(entry->d_name));
            *result = entry;
            return 0;
        }
    }

    void rewinddir(int &_errno_, DIR *dirp)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return;
        }
        auto vfs = purefs::subsystem::vfs_core();
        if (!vfs) {
            _errno_ = EIO;
            return;
        }
        auto res = vfs->dirreset(dirp->dirh);
        if (res < 0) {
            _errno_ = -res;
            return;
        }
        dirp->position = 0;
    }

    void seekdir(int &_errno_, DIR *dirp, long int loc)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return;
        }
        if (loc < 0) {
            _errno_ = EINVAL;
            return;
        }
        auto vfs = purefs::subsystem::vfs_core();
        if (!vfs) {
            _errno_ = EIO;
            return;
        }
        if (long(dirp->position) > loc) {
            vfs->dirreset(dirp->dirh);
            dirp->position = 0;
        }
        std::string name;
        struct stat st;
        while ((long(dirp->position) < loc) && (vfs->dirnext(dirp->dirh, name, st)) >= 0) {
            dirp->position += 1;
        }
    }

    long int telldir(int &_errno_, DIR *dirp)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return -1;
        }
        return dirp->position;
    }

    int chmod(int &_errno_, const char *path, mode_t mode)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::chmod, path, mode);
    }

    int fchmod(int &_errno_, int fd, mode_t mode)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::fchmod, fd, mode);
    }

    int fsync(int &_errno_, int fd)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::fsync, fd);
    }

    int statvfs(int &_errno_, const char *path, struct statvfs *buf)
    {
        if (!buf) {
            _errno_ = EINVAL;
            return -1;
        }
        else {
            return invoke_fs(_errno_, &purefs::fs::filesystem::stat_vfs, path, *buf);
        }
    }

    int umount(int &_errno_, const char *special_file)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::umount, special_file);
    }

    int mount(int &_errno_,
              const char *special_file,
              const char *dir,
              const char *fstype,
              unsigned long int rwflag,
              const void *data)
    {
        return invoke_fs(_errno_,
                         &purefs::fs::filesystem::mount,
                         special_file ? special_file : "",
                         dir ? dir : "",
                         fstype ? fstype : "",
                         rwflag,
                         data);
    }

    ssize_t readlink(int &_errno_, const char *path, char *buf, size_t buflen)
    {
        // Symlinks are not supported. According to man(2) symlink EINVAL when is not symlink.
        _errno_ = EINVAL;
        return -1;
    }

    int symlink(int &_errno_, const char *name1, const char *name2)
    {
        return invoke_fs(_errno_, &purefs::fs::filesystem::symlink, name1, name2);
    }

    long fpathconf(int &_errno, int fd, int name)
    {
        if (name == _PC_PATH_MAX) {
            return PATH_MAX;
        }
        else {
            _errno = EINVAL;
            return -1;
        }
    }

    long pathconf(int &_errno, const char *path, int name)
    {
        if (name == _PC_PATH_MAX) {
            return PATH_MAX;
        }
        else {
            _errno = EINVAL;
            return -1;
        }
    }

} // namespace vfsn::internal::syscalls
