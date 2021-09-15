// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "iosyscalls-internal.hpp"

#include <errno.h>
#include <fcntl.h>
#include <limits.h> // for PATH_MAX
#include <poll.h>
#include <stdarg.h> // for va_*
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>

#include "syscalls_real.hpp"

#include "debug.hpp"

namespace
{
    namespace real
    {
        __REAL_DECL(link);
        __REAL_DECL(unlink);
        __REAL_DECL(rmdir);
        __REAL_DECL(symlink);

        __REAL_DECL(fcntl);
#if __GLIBC__ > 2 || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 28))
        __REAL_DECL(fcntl64);
#endif
        __REAL_DECL(chdir);
        __REAL_DECL(fchdir);

        __REAL_DECL(getcwd);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        __REAL_DECL(getwd);
#pragma GCC diagnostic pop

        __REAL_DECL(get_current_dir_name);
        __REAL_DECL(mkdir);

        __REAL_DECL(chmod);
        __REAL_DECL(fchmod);
        __REAL_DECL(fsync);
        __REAL_DECL(fdatasync);

#if __GLIBC__ > 2 || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 33))
        __REAL_DECL(stat);
        __REAL_DECL(lstat);
        __REAL_DECL(fstat);

        __REAL_DECL(stat64);
        __REAL_DECL(lstat64);
        __REAL_DECL(fstat64);
#else
        __REAL_DECL(__xstat);
        __REAL_DECL(__lxstat);
        __REAL_DECL(__fxstat);

        __REAL_DECL(__xstat64);
        __REAL_DECL(__lxstat64);
        __REAL_DECL(__fxstat64);
#endif
        __REAL_DECL(read);
        __REAL_DECL(write);
        __REAL_DECL(lseek);
        __REAL_DECL(lseek64);

        __REAL_DECL(mount);
        __REAL_DECL(umount);

        __REAL_DECL(ioctl);
        __REAL_DECL(poll);
        __REAL_DECL(statvfs);
    } // namespace real

    void __attribute__((constructor)) _lib_posix_initialize()
    {
        __REAL_DLSYM(link);
        __REAL_DLSYM(unlink);
        __REAL_DLSYM(rmdir);
        __REAL_DLSYM(symlink);

        __REAL_DLSYM(fcntl);
#if __GLIBC__ > 2 || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 28))
        __REAL_DLSYM(fcntl64);
#endif
        __REAL_DLSYM(chdir);
        __REAL_DLSYM(fchdir);

        __REAL_DLSYM(getcwd);
        __REAL_DLSYM(getwd);
        __REAL_DLSYM(get_current_dir_name);
        __REAL_DLSYM(mkdir);

        __REAL_DLSYM(chmod);
        __REAL_DLSYM(fchmod);
        __REAL_DLSYM(fsync);
        __REAL_DLSYM(fdatasync);

#if __GLIBC__ > 2 || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 33))
        __REAL_DLSYM(stat);
        __REAL_DLSYM(lstat);
        __REAL_DLSYM(fstat);

        __REAL_DLSYM(stat64);
        __REAL_DLSYM(lstat64);
        __REAL_DLSYM(fstat64);
#else
        __REAL_DLSYM(__xstat);
        __REAL_DLSYM(__lxstat);
        __REAL_DLSYM(__fxstat);

        __REAL_DLSYM(__xstat64);
        __REAL_DLSYM(__lxstat64);
        __REAL_DLSYM(__fxstat64);
#endif
        __REAL_DLSYM(read);
        __REAL_DLSYM(write);
        __REAL_DLSYM(lseek);
        __REAL_DLSYM(lseek64);

        __REAL_DLSYM(mount);
        __REAL_DLSYM(umount);

        __REAL_DLSYM(ioctl);
        __REAL_DLSYM(poll);
        __REAL_DLSYM(statvfs);

        if (!(real::link && real::unlink && real::rmdir && real::symlink && real::fcntl && real::chdir &&
              real::fchdir && real::getcwd && real::getwd && real::get_current_dir_name && real::mkdir && real::chmod &&
              real::fchmod && real::fsync && real::fdatasync && real::read && real::write && real::lseek &&
              real::lseek64 && real::mount && real::umount && real::ioctl && real::poll && real::statvfs
#if __GLIBC__ > 2 || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 28))
              && real::fcntl64
#endif
#if __GLIBC__ > 2 || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 33))
              && real::stat && real::lstat && real::fstat && real::stat64 && real::lstat64 && real::fstat64
#else
              && real::__xstat && real::__lxstat && real::__fxstat && real::__xstat64 && real::__lxstat64 &&
              real::__fxstat64
#endif
              )) {
            abort();
        }
    }
} // namespace

extern "C"
{
    namespace vfs = vfsn::linux::internal;
    using fs      = purefs::fs::filesystem;

    int _iosys_read(int __fd, void *__buf, size_t __nbyte)
    {
        int ret;
        if (vfs::is_image_fd(__fd)) {
            TRACE_SYSCALLN("(%d, %p, %u) -> VFS", __fd, __buf, __nbyte);
            ret = vfs::invoke_fs(&fs::read, vfs::to_image_fd(__fd), (char *)__buf, __nbyte);
        }
        else {
            TRACE_SYSCALLN("(%d, %p, %u) -> linux fs", __fd, __buf, __nbyte);
            // returning result via a variable causes strange
            // sanitizer errors on debug build
            return real::read(__fd, __buf, __nbyte);
        }
        TRACE_SYSCALLN("(%d, %p, %u) = %d", __fd, __buf, __nbyte, ret);
        return ret;
    }
    __asm__(".symver _iosys_read,read@GLIBC_2.2.5");

    int _iosys_write(int __fd, const void *__buf, size_t __nbyte)
    {
        int ret;
        if (vfs::is_image_fd(__fd)) {
            TRACE_SYSCALLN("(%d, %p, %u) -> VFS", __fd, __buf, __nbyte);
            ret = vfs::invoke_fs(&fs::write, vfs::to_image_fd(__fd), (const char *)__buf, __nbyte);
        }
        else {
            TRACE_SYSCALLN("(%d, %p, %u) -> linux fs", __fd, __buf, __nbyte);
            ret = real::write(__fd, __buf, __nbyte);
        }
        TRACE_SYSCALLN("(%d, %p, %u) = %d", __fd, __buf, __nbyte, ret);
        return ret;
    }
    __asm__(".symver _iosys_write,write@GLIBC_2.2.5");

    int _iosys_lseek(int __fildes, off_t __offset, int __whence)
    {
        if (vfs::is_image_fd(__fildes)) {
            TRACE_SYSCALLN("(%d) -> VFS", __fildes);
            return vfs::invoke_fs(&fs::seek, vfs::to_image_fd(__fildes), __offset, __whence);
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", __fildes);
            return real::lseek(__fildes, __offset, __whence);
        }
    }
    __asm__(".symver _iosys_lseek,lseek@GLIBC_2.2.5");

    int _iosys_lseek64(int __fd, loff_t __offset, int __whence)
    {
        if (vfs::is_image_fd(__fd)) {
            TRACE_SYSCALLN("%s", "-> lseek");
            return lseek(__fd, __offset, __whence);
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", __fd);
            return real::lseek64(__fd, __offset, __whence);
        }
    }
    __asm__(".symver _iosys_lseek64,lseek64@GLIBC_2.2.5");

    int _iosys_ioctl(int __fd, unsigned long int __request, ...)
    {
        int ret;
        uintptr_t param;
        va_list args;
        va_start(args, __request);
        param = va_arg(args, uintptr_t);
        if (vfs::is_image_fd(__fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", __fd);
            // VFS's ioctl works on paths, not file descriptors
            // so we can't handle it the easy way, f ex.
            // ret = vfs::invoke_fs(__VFS(ioctl), __fd, __request, param);
            errno = ENOTSUP;
            std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
            ret = -1;
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", __fd);
            ret = real::ioctl(__fd, __request, param);
        }
        va_end(args);
        return ret;
    }
    __asm__(".symver _iosys_ioctl,ioctl@GLIBC_2.2.5");

    int _iosys_poll(struct pollfd *__fds, nfds_t __nfds, int __timeout)
    {
        if (__fds && vfs::is_image_fd(__fds->fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", __fds->fd);
            // there's no point in translating this call
            errno = ENOTSUP;
            std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
            return -1;
        }
        TRACE_SYSCALLN("(%d) -> linux fs", __fds ? __fds->fd : 0);
        return real::poll(__fds, __nfds, __timeout);
    }
    __asm__(".symver _iosys_poll,poll@GLIBC_2.2.5");

    int _iosys_link(const char *oldpath, const char *newpath)
    {
        if (vfs::redirect_to_image(oldpath)) {
            TRACE_SYSCALLN("(%s,%s) -> VFS", oldpath, newpath);
            errno = ENOSYS;
            std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
            return -1;
        }
        else {
            TRACE_SYSCALLN("(%s,%s) -> linux fs", oldpath, newpath);
            char tmp[PATH_MAX], tmp2[PATH_MAX];
            const auto oldp = vfs::npath_translate(oldpath, tmp);
            const auto newp = vfs::npath_translate(newpath, tmp2);
            return real::link(oldp, newp);
        }
    }
    __asm__(".symver _iosys_link,link@GLIBC_2.2.5");

    int _iosys_unlink(const char *name)
    {
        if (vfs::redirect_to_image(name)) {
            TRACE_SYSCALLN("(%s) -> VFS", name);
            return vfs::invoke_fs(&fs::unlink, name);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", name);
            char tmp[PATH_MAX];
            const auto path = vfs::npath_translate(name, tmp);
            return real::unlink(path);
        }
    }
    __asm__(".symver _iosys_unlink,unlink@GLIBC_2.2.5");

    int _iosys_rmdir(const char *name)
    {
        if (vfs::redirect_to_image(name)) {
            TRACE_SYSCALLN("(%s) -> VFS", name);
            return vfs::invoke_fs(&fs::rmdir, name);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", name);
            char tmp[PATH_MAX];
            const auto path = vfs::npath_translate(name, tmp);
            return real::rmdir(path);
        }
    }
    __asm__(".symver _iosys_rmdir,rmdir@GLIBC_2.2.5");

    int _iosys_fcntl(int fd, int cmd, ... /* arg */)
    {
        if (vfs::is_image_fd(fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", fd);
            errno = ENOSYS;
            std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
            return -1;
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", fd);
            uintptr_t param;
            va_list args;
            va_start(args, cmd);
            param    = va_arg(args, uintptr_t);
            auto ret = real::fcntl(fd, cmd, param);
            va_end(args);
            return ret;
        }
    }
    __asm__(".symver _iosys_fcntl,fcntl@GLIBC_2.2.5");

#if __GLIBC__ > 2 || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 28))
    int _iosys_fcntl64(int fd, int cmd, ... /* arg */)
    {
        if (vfs::is_image_fd(fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", fd);
            errno = ENOSYS;
            std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
            return -1;
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", fd);
            uintptr_t param;
            va_list args;
            va_start(args, cmd);
            param    = va_arg(args, uintptr_t);
            auto ret = real::fcntl64(fd, cmd, param);
            va_end(args);
            return ret;
        }
    }
    __asm__(".symver _iosys_fcntl64,fcntl64@GLIBC_2.2.5");
#endif

    int _iosys_chdir(const char *path)
    {
        if (vfs::redirect_to_image(path)) {
            TRACE_SYSCALLN("(%s) -> VFS", path);
            return vfs::invoke_fs(&fs::chdir, path);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", path);
            char tmp[PATH_MAX];
            const auto newpath = vfs::npath_translate(path, tmp);
            return real::chdir(newpath);
        }
    }
    __asm__(".symver _iosys_chdir,chdir@GLIBC_2.2.5");

    int _iosys_fchdir(int fd)
    {
        if (vfs::is_image_fd(fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", fd);
            errno = ENOSYS;
            std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
            return -1;
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", fd);
            return real::fchdir(fd);
        }
    }
    __asm__(".symver _iosys_fchdir,fchdir@GLIBC_2.2.5");

    char *_iosys_getcwd(char *buf, size_t size)
    {
        if (vfs::redirect_to_image()) {
            TRACE_SYSCALLN("%s", "() -> VFS");
            auto vfs = purefs::subsystem::vfs_core();
            if (!vfs) {
                errno = EIO;
                return nullptr;
            }
            auto xwd = vfs->getcwd();
            xwd.copy(buf, size);
            return buf;
        }
        else {
            TRACE_SYSCALLN("%s", "() -> linux fs");
            return real::getcwd(buf, size);
        }
    }
    __asm__(".symver _iosys_getcwd,getcwd@GLIBC_2.2.5");

    char *_iosys_getwd(char *buf)
    {
        if (vfs::redirect_to_image()) {
            TRACE_SYSCALLN("%s", "() -> VFS");
            auto vfs = purefs::subsystem::vfs_core();
            if (!vfs) {
                errno = EIO;
                return nullptr;
            }
            auto xwd = vfs->getcwd();
            xwd.copy(buf, xwd.size());
            return buf;
        }
        else {
            TRACE_SYSCALLN("%s", "() -> linux fs");
            return real::getwd(buf);
        }
    }
    __asm__(".symver _iosys_getwd,getwd@GLIBC_2.2.5");

    char *_iosys_get_current_dir_name(void)
    {
        if (vfs::redirect_to_image()) {
            TRACE_SYSCALLN("%s", "() -> VFS");
            auto vfs = purefs::subsystem::vfs_core();
            if (!vfs) {
                errno = EIO;
                return nullptr;
            }
            auto xwd = vfs->getcwd();
            auto ret = new char[xwd.size() + 1];
            xwd.copy(ret, xwd.size());
            ret[xwd.size()] = '\0';
            return ret;
        }
        else {
            TRACE_SYSCALLN("%s", "() -> linux fs");
            return real::get_current_dir_name();
        }
    }
    __asm__(".symver _iosys_get_current_dir_name,get_current_dir_name@GLIBC_2.2.5");

    int _iosys_mkdir(const char *pathname, mode_t mode)
    {
        if (vfs::redirect_to_image(pathname)) {
            TRACE_SYSCALLN("(%s) -> VFS", pathname);
            return vfs::invoke_fs(&fs::mkdir, pathname, mode);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", pathname);
            char tmp[PATH_MAX];
            const auto path = vfs::npath_translate(pathname, tmp);
            return real::mkdir(path, mode);
        }
    }
    __asm__(".symver _iosys_mkdir,mkdir@GLIBC_2.2.5");

    int _iosys_chmod(const char *pathname, mode_t mode)
    {
        if (vfs::redirect_to_image(pathname)) {
            TRACE_SYSCALLN("(%s) -> VFS", pathname);
            return vfs::invoke_fs(&fs::chmod, pathname, mode);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", pathname);
            char tmp[PATH_MAX];
            const auto path = vfs::npath_translate(pathname, tmp);
            return real::chmod(path, mode);
        }
    }
    __asm__(".symver _iosys_chmod,chmod@GLIBC_2.2.5");

    int _iosys_fchmod(int fd, mode_t mode)
    {
        if (vfs::is_image_fd(fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", fd);
            return vfs::invoke_fs(&fs::fchmod, vfs::to_image_fd(fd), mode);
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", fd);
            return real::fchmod(fd, mode);
        }
    }
    __asm__(".symver _iosys_fchmod,fchmod@GLIBC_2.2.5");

    int _iosys_fsync(int fd)
    {
        if (vfs::is_image_fd(fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", fd);
            return vfs::invoke_fs(&fs::fsync, vfs::to_image_fd(fd));
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", fd);
            return real::fsync(fd);
        }
    }
    __asm__(".symver _iosys_fsync,fsync@GLIBC_2.2.5");

    int _iosys_fdatasync(int fd)
    {
        if (vfs::is_image_fd(fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", fd);
            return vfs::invoke_fs(&fs::fsync, vfs::to_image_fd(fd));
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", fd);
            return real::fdatasync(fd);
        }
    }
    __asm__(".symver _iosys_fdatasync,fdatasync@GLIBC_2.2.5");

    int _iosys_symlink(const char *target, const char *linkpath)
    {
        if (vfs::redirect_to_image(target)) {
            TRACE_SYSCALLN("(%s,%s) -> VFS", target, linkpath);
            return vfs::invoke_fs(&fs::symlink, target, linkpath);
        }
        else {
            TRACE_SYSCALLN("(%s,%s) -> linux fs", target, linkpath);
            char tmp[PATH_MAX], tmp2[PATH_MAX];
            const auto tgtp = vfs::npath_translate(target, tmp);
            const auto linp = vfs::npath_translate(linkpath, tmp2);
            return real::symlink(tgtp, linp);
        }
    }
    __asm__(".symver _iosys_symlink,symlink@GLIBC_2.2.5");

#if __GLIBC__ > 2 || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 33))
    int _iosys_stat(const char *file, struct stat *pstat)
    {
        if (vfs::redirect_to_image(file)) {
            TRACE_SYSCALLN("(%s) -> VFS", file);
            return vfs::invoke_fs(&fs::stat, file, *pstat);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", file);
            char tmp[PATH_MAX];
            const auto newfile = vfs::npath_translate(file, tmp);
            return real::stat(newfile, pstat);
        }
    }
    __asm__(".symver _iosys_stat,stat@GLIBC_2.33");

    int _iosys_lstat(const char *pathname, struct stat *statbuf)
    {
        if (vfs::redirect_to_image(pathname)) {
            TRACE_SYSCALLN("(%s) -> VFS", pathname);
            return vfs::invoke_fs(&fs::stat, pathname, *statbuf);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", pathname);
            char tmp[PATH_MAX];
            const auto newpath = vfs::npath_translate(pathname, tmp);
            return real::lstat(newpath, statbuf);
        }
    }
    __asm__(".symver _iosys_lstat,lstat@GLIBC_2.33");

    int _iosys_fstat(int fd, struct stat *pstat)
    {
        if (vfs::is_image_fd(fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", fd);
            return vfs::invoke_fs(&fs::fstat, vfs::to_image_fd(fd), *pstat);
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", fd);
            return real::fstat(fd, pstat);
        }
    }
    __asm__(".symver _iosys_fstat,fstat@GLIBC_2.33");

    int _iosys_stat64(const char *file, struct stat64 *pstat)
    {
        if (vfs::redirect_to_image(file)) {
            TRACE_SYSCALLN("(%s) -> VFS", file);
            return stat(file, (struct stat *)pstat);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", file);
            char tmp[PATH_MAX];
            const auto newfile = vfs::npath_translate(file, tmp);
            return real::stat64(newfile, pstat);
        }
    }
    __asm__(".symver _iosys_stat64,stat64@GLIBC_2.33");

    int _iosys_lstat64(const char *pathname, struct stat64 *statbuf)
    {
        if (vfs::redirect_to_image(pathname)) {
            TRACE_SYSCALLN("(%s) -> VFS", pathname);
            return lstat(pathname, (struct stat *)statbuf);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", pathname);
            char tmp[PATH_MAX];
            const auto newpath = vfs::npath_translate(pathname, tmp);
            return real::lstat64(newpath, statbuf);
        }
    }
    __asm__(".symver _iosys_lstat64,lstat64@GLIBC_2.33");

    int _iosys_fstat64(int fd, struct stat64 *pstat)
    {
        if (vfs::is_image_fd(fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", fd);
            return fstat(fd, (struct stat *)pstat);
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", fd);
            return real::fstat64(fd, pstat);
        }
    }
    __asm__(".symver _iosys_fstat64,fstat64@GLIBC_2.33");

#else
    int _iosys_xstat(int ver, const char *path, struct stat *stat_buf)
    {
        if (vfs::redirect_to_image(path)) {
            TRACE_SYSCALLN("(%s) -> VFS", path);
            return vfs::invoke_fs(&fs::stat, path, *stat_buf);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", path);
            char tmp[PATH_MAX];
            const auto newp = vfs::npath_translate(path, tmp);
            return real::__xstat(ver, newp, stat_buf);
        }
    }
    __asm__(".symver _iosys_xstat,__xstat@GLIBC_2.2.5");

    int _iosys_lxstat(int ver, const char *path, struct stat *stat_buf)
    {
        if (vfs::redirect_to_image(path)) {
            TRACE_SYSCALLN("(%s) -> VFS", path);
            return vfs::invoke_fs(&fs::stat, path, *stat_buf);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", path);
            char tmp[PATH_MAX];
            const auto newp = vfs::npath_translate(path, tmp);
            return real::__lxstat(ver, newp, stat_buf);
        }
    }
    __asm__(".symver _iosys_lxstat,__lxstat@GLIBC_2.2.5");

    int _iosys_fxstat(int ver, int fildes, struct stat *stat_buf)
    {
        if (vfs::is_image_fd(fildes)) {
            TRACE_SYSCALLN("(%d) -> VFS", fildes);
            return vfs::invoke_fs(&fs::fstat, vfs::to_image_fd(fildes), *stat_buf);
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", fildes);
            return real::__fxstat(ver, fildes, stat_buf);
        }
    }
    __asm__(".symver _iosys_fxstat,__fxstat@GLIBC_2.2.5");

    int _iosys_xstat64(int ver, const char *path, struct stat64 *stat_buf)
    {
        if (vfs::redirect_to_image(path)) {
            TRACE_SYSCALLN("(%s) -> VFS", path);
            return vfs::invoke_fs(&fs::stat, path, *(struct stat *)stat_buf);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", path);
            char tmp[PATH_MAX];
            const auto newp = vfs::npath_translate(path, tmp);
            return real::__xstat64(ver, newp, stat_buf);
        }
    }
    __asm__(".symver _iosys_xstat64,__xstat64@GLIBC_2.2.5");

    int _iosys_lxstat64(int ver, const char *path, struct stat64 *stat_buf)
    {
        if (vfs::redirect_to_image(path)) {
            TRACE_SYSCALLN("(%s) -> VFS", path);
            return vfs::invoke_fs(&fs::stat, path, *(struct stat *)stat_buf);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", path);
            char tmp[PATH_MAX];
            const auto newp = vfs::npath_translate(path, tmp);
            return real::__lxstat64(ver, newp, stat_buf);
        }
    }
    __asm__(".symver _iosys_lxstat64,__lxstat64@GLIBC_2.2.5");

    int _iosys_fxstat64(int ver, int fildes, struct stat64 *stat_buf)
    {
        if (vfs::is_image_fd(fildes)) {
            TRACE_SYSCALLN("(%d) -> VFS", fildes);
            return vfs::invoke_fs(&fs::fstat, vfs::to_image_fd(fildes), *(struct stat *)stat_buf);
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", fildes);
            return real::__fxstat64(ver, fildes, stat_buf);
        }
    }
    __asm__(".symver _iosys_fxstat64,__fxstat64@GLIBC_2.2.5");
#endif

    int _iosys_fxstatat(int vers, int fd, const char *filename, struct stat *buf, int flag)
    {
        errno = ENOSYS;
        std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
        return -1;
    }
    __asm__(".symver _iosys_fxstatat,__fxstatat@GLIBC_2.4");

    int _iosys_fxstatat64(int vers, int fd, const char *filename, struct stat64 *buf, int flag)
    {
        errno = ENOSYS;
        std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
        return -1;
    }
    __asm__(".symver _iosys_fxstatat64,__fxstatat64@GLIBC_2.4");

    int _iosys_mount(
        const char *special_file, const char *dir, const char *fstype, unsigned long int rwflag, const void *data)
    {
        if (vfs::redirect_to_image(dir)) {
            TRACE_SYSCALLN("(%s, %s, %s, %08lx, %p) -> VFS",
                           special_file ? special_file : "(null)",
                           dir ? dir : "(null)",
                           fstype ? fstype : "(null)",
                           rwflag,
                           data);
            return vfs::invoke_fs(
                &fs::mount, special_file ? special_file : "", dir ? dir : "", fstype ? fstype : "", rwflag, data);
        }
        else {
            TRACE_SYSCALLN("(%s, %s, %s, %08lx,%p) -> linux fs", special_file, dir, fstype, rwflag, data);
            return real::mount(special_file, dir, fstype, rwflag, data);
        }
    }
    __asm__(".symver _iosys_mount,mount@GLIBC_2.2.5");

    int _iosys_umount(const char *mount_point)
    {
        if (vfs::redirect_to_image(mount_point)) {
            TRACE_SYSCALLN("(%s) -> VFS", mount_point);
            return vfs::invoke_fs(&fs::umount, mount_point);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", mount_point);
            return real::umount(mount_point);
        }
    }
    __asm__(".symver _iosys_umount,umount@GLIBC_2.2.5");

    int _iosys_statvfs(const char *path, struct statvfs *vfstat)
    {
        TRACE_SYSCALL();
        if (vfs::redirect_to_image(path)) {
            TRACE_SYSCALLN("(%s) -> VFS", path);
            return vfs::invoke_fs(&fs::stat_vfs, path, *vfstat);
        }
        else {
            TRACE_SYSCALLN("(%s) -> linux fs", path);
            char tmp[PATH_MAX];
            const auto newp = vfs::npath_translate(path, tmp);
            return real::statvfs(newp, vfstat);
        }
    }
    __asm__(".symver _iosys_statvfs,statvfs@GLIBC_2.2.5");
}
