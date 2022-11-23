// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "iosyscalls-internal.hpp"

#include <dirent.h>
#include <errno.h>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <bits/fcntl2.h>

#include "syscalls_real.hpp"

#include "debug.hpp"

struct __dirstream
{
    purefs::fs::filesystem::fsdir dirh;
    struct dirent dir_data;
    size_t position;
};

namespace
{
    namespace real
    {
        __REAL_DECL(opendir);
        __REAL_DECL(__openat_2);
        __REAL_DECL(closedir);
        __REAL_DECL(readdir);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        __REAL_DECL(readdir_r);
#pragma GCC diagnostic pop
        __REAL_DECL(rewinddir);
        __REAL_DECL(seekdir);
        __REAL_DECL(telldir);
    } // namespace real

    void __attribute__((constructor)) _lib_posix_dirent_initialize()
    {
        __REAL_DLSYM(opendir);
        __REAL_DLSYM(__openat_2);
        __REAL_DLSYM(closedir);
        __REAL_DLSYM(readdir);
        __REAL_DLSYM(readdir_r);
        __REAL_DLSYM(rewinddir);
        __REAL_DLSYM(seekdir);
        __REAL_DLSYM(telldir);

        if (!(real::opendir && real::__openat_2 && real::closedir && real::readdir && real::readdir_r &&
              real::rewinddir && real::seekdir && real::telldir)) {
            abort();
        }
    }
} // namespace

extern "C"
{
    namespace vfs = vfsn::linux::internal;

    DIR *_iosys_opendir(const char *dirname)
    {
        __dirstream *ret{};
        if (!dirname) {
            TRACE_SYSCALLN("(%p) invalid argument", dirname);
            errno = EINVAL;
            return ret;
        }
        if (vfs::redirect_to_image(dirname)) {
            TRACE_SYSCALLN("(%s) -> VFS", dirname);
            auto vfs = purefs::subsystem::vfs_core();
            if (!vfs) {
                errno = EIO;
            }
            else {
                ret = new (std::nothrow) __dirstream;
                if (!ret) {
                    errno = ENOMEM;
                }
                else {
                    ret->position = 0;
                    ret->dirh     = vfs->diropen(dirname);
                    if (!ret->dirh) {
                        delete ret;
                        errno = EIO;
                        ret   = nullptr;
                    }
                    else if (ret->dirh->error()) {
                        errno = -ret->dirh->error();
                        delete ret;
                        ret = nullptr;
                    }
                }
            }
            if (ret) {
                vfs::add_DIR_to_image_list(ret);
            }
        }
        else {
            char tmp[PATH_MAX];
            const auto newpath = vfs::npath_translate(dirname, tmp);
            TRACE_SYSCALLN("(%s) -> (%s) linux fs", dirname, newpath);
            ret = real::opendir(newpath);
        }
        TRACE_SYSCALLN("(%s)=%p errno=%i", dirname, ret, errno);
        return ret;
    }
    __asm__(".symver _iosys_opendir,opendir@GLIBC_2.2.5");

    int _iosys__openat_2(int fd, const char *dirname, int flags)
    {
        int ret = 0;
        if (!dirname) {
            TRACE_SYSCALLN("(%p) invalid argument", dirname);
            errno = EINVAL;
            return -1;
        }
        if (vfs::redirect_to_image(dirname)) {
            std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
        }
        else {
            char tmp[PATH_MAX];
            const auto newpath = vfs::npath_translate(dirname, tmp);
            TRACE_SYSCALLN("(%s) -> (%s) linux fs", dirname, newpath);
            ret = real::__openat_2(fd, newpath, flags);
        }
        TRACE_SYSCALLN("(%s)=%p errno=%i", dirname, ret, errno);
        return ret;
    }
    __asm__(".symver _iosys__openat_2,__openat_2@GLIBC_2.7");

    int _iosys_closedir(DIR *dirp)
    {
        int ret{};
        if (!dirp) {
            TRACE_SYSCALLN("(%p) invalid argument", dirp);
            errno = EBADF;
            return -1;
        }
        if (vfs::is_image_DIR(dirp)) {
            TRACE_SYSCALLN("(%p) -> VFS", dirp);
            auto vfs = purefs::subsystem::vfs_core();
            if (!vfs) {
                errno = EIO;
                return -1;
            }
            ret = vfs->dirclose(dirp->dirh);
            if (ret < 0) {
                errno = -ret;
                ret   = -1;
            }
            vfs::remove_DIR_from_image_list(dirp);
            delete dirp;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", dirp);
            ret = real::closedir(dirp);
        }
        TRACE_SYSCALLN("(%p)=%i errno=%i", dirp, ret, errno);
        return ret;
    }
    __asm__(".symver _iosys_closedir,closedir@GLIBC_2.2.5");

    struct dirent *_iosys_readdir(DIR *dirp)
    {
        dirent *ret{};
        if (!dirp) {
            TRACE_SYSCALLN("(%p) invalid argument", dirp);
            errno = EBADF;
            return ret;
        }
        if (vfs::is_image_DIR(dirp)) {
            TRACE_SYSCALLN("(%p) -> VFS", dirp);
            auto vfs = purefs::subsystem::vfs_core();
            if (!vfs) {
                errno = EIO;
                return ret;
            }
            std::string fname;
            struct stat stdata;
            auto res = vfs->dirnext(dirp->dirh, fname, stdata);
            if (res < 0) {
                if (res != -ENODATA) {
                    errno = -res;
                }
                return ret;
            }
            else {
                if (fname.size() >= sizeof(dirp->dir_data.d_name)) {
                    errno = EOVERFLOW;
                    return ret;
                }
                dirp->position += 1;
                dirp->dir_data.d_ino    = stdata.st_ino;
                dirp->dir_data.d_type   = S_ISREG(stdata.st_mode) ? DT_REG : DT_DIR;
                dirp->dir_data.d_reclen = fname.size();
                std::strncpy(dirp->dir_data.d_name, fname.c_str(), sizeof(dirp->dir_data.d_name) - 1);
                ret = &dirp->dir_data;
            }
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", dirp);
            ret = real::readdir(dirp);
        }
        TRACE_SYSCALLN("(%p)=%p errno=%i", dirp, ret, errno);
        return ret;
    }
    __asm__(".symver _iosys_readdir,readdir@GLIBC_2.2.5");

    int _iosys_readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
    {
        if (!dirp) {
            TRACE_SYSCALLN("(%p) invalid argument", dirp);
            errno = EBADF;
            return -1;
        }
        if (vfs::is_image_DIR(dirp)) {
            TRACE_SYSCALLN("(%p) -> VFS", dirp);
            auto vfs = purefs::subsystem::vfs_core();
            if (!vfs) {
                errno = EIO;
                return -1;
            }
            std::string fname;
            struct stat stdata;
            auto res = vfs->dirnext(dirp->dirh, fname, stdata);
            if (res < 0) {
                errno = -res;
                res   = -1;
                return res;
            }
            else {
                if (fname.size() >= sizeof(entry->d_name)) {
                    errno = EOVERFLOW;
                    return -1;
                }
                dirp->position += 1;
                entry->d_ino    = stdata.st_ino;
                entry->d_type   = S_ISREG(stdata.st_mode) ? DT_REG : DT_DIR;
                entry->d_reclen = fname.size();
                std::strncpy(entry->d_name, fname.c_str(), sizeof(entry->d_name) - 1);
                *result = entry;
                return 0;
            }
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", dirp);
            return real::readdir_r(dirp, entry, result);
        }
    }
    __asm__(".symver _iosys_readdir_r,readdir_r@GLIBC_2.2.5");

    void _iosys_rewinddir(DIR *dirp)
    {
        if (!dirp) {
            TRACE_SYSCALLN("(%p) invalid argument", dirp);
            errno = EBADF;
            return;
        }
        if (vfs::is_image_DIR(dirp)) {
            TRACE_SYSCALLN("(%p) -> VFS", dirp);
            auto vfs = purefs::subsystem::vfs_core();
            if (!vfs) {
                return;
            }
            auto res = vfs->dirreset(dirp->dirh);
            if (res < 0) {
                return;
            }
            dirp->position = 0;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", dirp);
            real::rewinddir(dirp);
        }
    }
    __asm__(".symver _iosys_rewinddir,rewinddir@GLIBC_2.2.5");

    void _iosys_seekdir(DIR *dirp, long int loc)
    {
        if (!dirp) {
            TRACE_SYSCALLN("(%p,%ld) invalid argument", dirp, loc);
            errno = EBADF;
            return;
        }
        if (vfs::is_image_DIR(dirp)) {
            TRACE_SYSCALLN("(%p,%ld) -> VFS", dirp, loc);
            if (loc < 0) {
                return;
            }
            auto vfs = purefs::subsystem::vfs_core();
            if (!vfs) {
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
        else {
            TRACE_SYSCALLN("(%p,%ld) -> linux fs", dirp, loc);
            real::seekdir(dirp, loc);
        }
    }
    __asm__(".symver _iosys_seekdir,seekdir@GLIBC_2.2.5");

    long int _iosys_telldir(DIR *dirp)
    {
        if (!dirp) {
            TRACE_SYSCALLN("(%p) invalid argument", dirp);
            errno = EBADF;
            return -1;
        }
        if (vfs::is_image_DIR(dirp)) {
            TRACE_SYSCALLN("(%p) -> VFS", dirp);
            return dirp->position;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", dirp);
            return real::telldir(dirp);
        }
    }
    __asm__(".symver _iosys_telldir,telldir@GLIBC_2.2.5");
}
