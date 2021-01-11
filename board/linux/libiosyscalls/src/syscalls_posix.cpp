// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <iosyscalls.hpp>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>

#include <stdarg.h>     // for va_*
#include <limits.h>     // for PATH_MAX
#include <dlfcn.h>      // for dlsym()

#include <stdio.h>

#include <purefs/fs/filesystem.hpp>

#include "debug.hpp"

#define __REAL_DECL(fun) decltype(::fun) *fun
#define __REAL_DLSYM(fun) real::fun = reinterpret_cast<decltype(real::fun)>(dlsym(RTLD_NEXT, #fun))

#define __VFS(fun) (&purefs::fs::filesystem::fun)

namespace
{
    namespace real {
        __REAL_DECL(link);
        __REAL_DECL(unlink);

        __REAL_DECL(fcntl);
        __REAL_DECL(fcntl64);

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
        __REAL_DECL(symlink);

        __REAL_DECL(__xstat);
        __REAL_DECL(__lxstat);
        __REAL_DECL(__fxstat);

        __REAL_DECL(__xstat64);
        __REAL_DECL(__lxstat64);
        __REAL_DECL(__fxstat64);

        __REAL_DECL(rename);
    } // namespace real

    void __attribute__((constructor)) _lib_posix_initialize()
    {
        __REAL_DLSYM(link);
        __REAL_DLSYM(unlink);

        __REAL_DLSYM(fcntl);
        __REAL_DLSYM(fcntl64);

        __REAL_DLSYM(chdir);
        __REAL_DLSYM(fchdir);

        __REAL_DLSYM(getcwd);
        __REAL_DLSYM(getwd);
        __REAL_DLSYM(get_current_dir_name);
        __REAL_DLSYM(mkdir);

        __REAL_DLSYM(chmod);
        __REAL_DLSYM(fchmod);
        __REAL_DLSYM(fsync);
        __REAL_DLSYM(symlink);

        __REAL_DLSYM(__xstat);
        __REAL_DLSYM(__lxstat);
        __REAL_DLSYM(__fxstat);

        __REAL_DLSYM(__xstat64);
        __REAL_DLSYM(__lxstat64);
        __REAL_DLSYM(__fxstat64);

        __REAL_DLSYM(rename);

        if (!(real::link && real::unlink
            && real::fcntl && real::fcntl64 && real::chdir && real::fchdir
            && real::getcwd && real::getwd && real::get_current_dir_name && real::mkdir
            && real::chmod && real::chdir && real::fchdir && real::fsync && real::symlink
            && real::__xstat && real::__lxstat && real::__fxstat
            && real::__xstat64 && real::__lxstat64 && real::__fxstat64
            && real::rename))
        {
            abort();
        }
    }
} //namespace

extern "C" {
    namespace vfs = vfsn::linux::internal;

    int _iosys_link(const char *oldpath, const char *newpath)
    {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image(oldpath))
        {
            errno = ENOSYS;
            std::cerr << "Unsupported syscall " <<  __PRETTY_FUNCTION__ << std::endl;
            return -1;
        }
        else
        {
            char tmp[PATH_MAX], tmp2[PATH_MAX];
            const auto oldp = vfs::npath_translate(oldpath,tmp);
            const auto newp = vfs::npath_translate(newpath,tmp2);
            return real::link(oldp,newp);
        }
    }
    __asm__(".symver _iosys_link,link@GLIBC_2.2.5");

     int _iosys_unlink(const char *name)
     {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image(name))
        {
            return vfs::invoke_fs(__VFS(unlink), name);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto path = vfs::npath_translate(name,tmp);
            return real::unlink(path);
        }
     }
    __asm__(".symver _iosys_unlink,unlink@GLIBC_2.2.5");

    int _iosys_stat(const char *file, struct stat *pstat)
    {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image(file))
        {
            return vfs::invoke_fs(__VFS(stat), file, *pstat);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto newfile = vfs::npath_translate(file,tmp);
            return real::__xstat(1,newfile,pstat);
        }
    }
    __asm__(".symver _iosys_stat,stat@GLIBC_2.2.5");

    int _iosys_lstat(const char *pathname, struct stat *statbuf)
    {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image(pathname))
        {
            return vfs::invoke_fs(__VFS(stat), pathname, *statbuf);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto newpath = vfs::npath_translate(pathname,tmp);
            return real::__lxstat(1,newpath,statbuf);
        }
    }
    __asm__(".symver _iosys_lstat,lstat@GLIBC_2.2.5");

    int _iosys_fstat(int fd, struct stat *pstat)
    {
        TRACE_SYSCALL();
        if(vfs::is_image_fd(fd))
        {
            return vfs::invoke_fs(__VFS(fstat),fd,*pstat);
        }
        else
        {
            return real::__fxstat(1,fd,pstat);
        }
    }
    __asm__(".symver _iosys_fstat,fstat@GLIBC_2.2.5");

    int _iosys_fcntl(int fd, int cmd, ... /* arg */ )
    {
        TRACE_SYSCALL();
        if(vfs::is_image_fd(fd))
        {
            errno = ENOSYS;
            std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
            return -1;
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            uintptr_t param;
            va_list args;
            va_start(args,cmd);
            param = va_arg(args,uintptr_t);
            auto ret =  real::fcntl(fd, cmd, param );
            va_end(args);
            return ret;
        }
    }
    __asm__(".symver _iosys_fcntl,fcntl@GLIBC_2.2.5");

    int _iosys_fcntl64(int fd, int cmd, ... /* arg */ )
    {
        TRACE_SYSCALL();
        if(vfs::is_image_fd(fd))
        {
            errno = ENOSYS;
            std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
            return -1;
        } else {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            uintptr_t param;
            va_list args;
            va_start(args,cmd);
            param = va_arg(args,uintptr_t);
            auto ret =  real::fcntl64(fd, cmd, param);
            va_end(args);
            return ret;
        }
    }
    __asm__(".symver _iosys_fcntl64,fcntl64@GLIBC_2.2.5");

    int _iosys_chdir(const char *path)
    {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image(path))
        {
            return vfs::invoke_fs(__VFS(chdir), path);
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            char tmp[PATH_MAX];
            const auto newpath = vfs::npath_translate(path,tmp);
            return real::chdir(newpath);
        }
    }
    __asm__(".symver _iosys_chdir,chdir@GLIBC_2.2.5");

    int _iosys_fchdir(int fd)
    {
        TRACE_SYSCALL();
        if(vfs::is_image_fd(fd))
        {
            errno = ENOSYS;
            std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
            return -1;
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            return real::fchdir(fd);
        }
    }
    __asm__(".symver _iosys_fchdir,fchdir@GLIBC_2.2.5");

    char *_iosys_getcwd(char *buf, size_t size)
    {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image())
        {
            auto vfs = purefs::subsystem::vfs_core();
            if(!vfs) {
                errno = EIO;
                return nullptr;
            }
            auto xwd = vfs->getcwd();
            xwd.copy(buf,size);
            return buf;
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            return real::getcwd(buf,size);
        }
    }
    __asm__(".symver _iosys_getcwd,getcwd@GLIBC_2.2.5");

    char *_iosys_getwd(char *buf)
    {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image())
        {
            auto vfs = purefs::subsystem::vfs_core();
            if(!vfs) {
                errno = EIO;
                return nullptr;
            }
            auto xwd = vfs->getcwd();
            xwd.copy(buf,xwd.size());
            return buf;
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            return real::getwd(buf);
        }
    }
    __asm__(".symver _iosys_getwd,getwd@GLIBC_2.2.5");


    char *_iosys_get_current_dir_name(void)
    {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image())
        {
            auto vfs = purefs::subsystem::vfs_core();
            if(!vfs) {
                errno = EIO;
                return nullptr;
            }
            auto xwd = vfs->getcwd();
            auto ret = new char[xwd.size()+1];
            xwd.copy(ret,xwd.size());
            ret[xwd.size()] = '\0';
            return ret;
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            return real::get_current_dir_name();
        }
    }
    __asm__(".symver _iosys_get_current_dir_name,get_current_dir_name@GLIBC_2.2.5");

    int _iosys_rename(const char *oldpath, const char *newpath)
    {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image(oldpath))
        {
            return vfs::invoke_fs(__VFS(rename), oldpath, newpath);
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            char tmp[PATH_MAX], tmp2[PATH_MAX];
            const auto oldp = vfs::npath_translate(oldpath,tmp);
            const auto newp = vfs::npath_translate(newpath,tmp2);
            return real::rename(oldp,newp);
        }
    }
    __asm__(".symver _iosys_rename,rename@GLIBC_2.2.5");

    int _iosys_mkdir(const char *pathname, mode_t mode)
    {
        if(vfs::redirect_to_image(pathname))
        {
            return vfs::invoke_fs(__VFS(mkdir), pathname, mode);
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            char tmp[PATH_MAX];
            const auto path = vfs::npath_translate(pathname,tmp);
            return real::mkdir(path,mode);
        }
    }
    __asm__(".symver _iosys_mkdir,mkdir@GLIBC_2.2.5");

    int _iosys_chmod(const char *pathname, mode_t mode)
    {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image(pathname))
        {
            return vfs::invoke_fs(__VFS(chmod),pathname,mode);
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            char tmp[PATH_MAX];
            const auto path = vfs::npath_translate(pathname,tmp);
            return real::chmod(path,mode);
        }
    }
    __asm__(".symver _iosys_chmod,chmod@GLIBC_2.2.5");

    int _iosys_fchmod(int fd, mode_t mode)
    {
        TRACE_SYSCALL();
        if(vfs::is_image_fd(fd))
        {
            return vfs::invoke_fs(__VFS(fchmod),fd,mode);
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            return real::fchmod(fd,mode);
        }
    }
    __asm__(".symver _iosys_fchmod,fchmod@GLIBC_2.2.5");

    int _iosys_fsync(int fd)
    {
        TRACE_SYSCALL();
        if(vfs::is_image_fd(fd))
        {
            return vfs::invoke_fs(__VFS(fsync), fd);
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            return real::fsync(fd);
        }
    }
    __asm__(".symver _iosys_fsync,fsync@GLIBC_2.2.5");

    int _iosys_fdatasync(int fd)
    {
        TRACE_SYSCALL();
        return fsync(fd);
    }
    __asm__(".symver _iosys_fdatasync,fdatasync@GLIBC_2.2.5");

    int _iosys_symlink(const char *target, const char *linkpath)
    {
        TRACE_SYSCALL();
        if(vfs::redirect_to_image(target))
        {
            return vfs::invoke_fs(__VFS(symlink),target,linkpath);
        }
        else
        {
            TRACE_SYSCALLN("%s", "Redirecting to linux fs");
            char tmp[PATH_MAX], tmp2[PATH_MAX];
            const auto tgtp = vfs::npath_translate(target,tmp);
            const auto linp = vfs::npath_translate(linkpath,tmp2);
            return real::symlink(tgtp,linp);
        }
    }
    __asm__(".symver _iosys_symlink,symlink@GLIBC_2.2.5");

    int _iosys_xstat(int ver, const char * path, struct stat * stat_buf)
    {
        if(vfs::redirect_to_image(path))
        {
            return vfs::invoke_fs(__VFS(stat), path, *stat_buf);
        }
        else
        {
            TRACE_SYSCALLN("(%s) -> linux fs", path);
            char tmp[PATH_MAX];
            const auto newp = vfs::npath_translate(path,tmp);
            return real::__xstat(ver,newp,stat_buf);
        }
    }
    __asm__(".symver _iosys_xstat,__xstat@GLIBC_2.2.5");

    int _iosys_lxstat(int ver, const char * path, struct stat * stat_buf)
    {
        if(vfs::redirect_to_image(path))
        {
            return vfs::invoke_fs(__VFS(stat), path, *stat_buf);
        }
        else
        {
            TRACE_SYSCALLN("(%s) -> linux fs", path);
            char tmp[PATH_MAX];
            const auto newp = vfs::npath_translate(path,tmp);
            return real::__lxstat(ver,newp,stat_buf);
        }
    }
    __asm__(".symver _iosys_lxstat,__lxstat@GLIBC_2.2.5");

    int _iosys_fxstat(int ver, int fildes, struct stat * stat_buf)
    {
        if(vfs::is_image_fd(fildes))
        {
            return vfs::invoke_fs(__VFS(fstat), fildes, *stat_buf);
        }
        else
        {
            TRACE_SYSCALLN("(%d) -> linux fs", fildes);
            return real::__fxstat(ver,fildes,stat_buf);
        }
    }
    __asm__(".symver _iosys_fxstat,__fxstat@GLIBC_2.2.5");

    int _iosys_fxstatat(int vers, int fd, const char *filename, struct stat *buf, int flag)
    {
        errno = ENOSYS;
        std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
        return -1;
    }
    __asm__(".symver _iosys_fxstatat,__fxstatat@GLIBC_2.2.5");

    int _iosys_xstat64(int ver, const char * path, struct stat64 * stat_buf)
    {
        if(vfs::redirect_to_image(path))
        {
            return vfs::invoke_fs(__VFS(stat), path, *(struct stat*)stat_buf);
        }
        else
        {
            TRACE_SYSCALLN("(%s) -> linux fs", path);
            char tmp[PATH_MAX];
            const auto newp = vfs::npath_translate(path,tmp);
            return real::__xstat64(ver,newp,stat_buf);
        }
    }
    __asm__(".symver _iosys_xstat64,__xstat64@GLIBC_2.2.5");

    int _iosys_lxstat64(int ver, const char * path, struct stat64 * stat_buf)
    {
        if(vfs::redirect_to_image(path))
        {
            return vfs::invoke_fs(__VFS(stat), path, *(struct stat*)stat_buf);
        }
        else
        {
            TRACE_SYSCALLN("(%s) -> linux fs", path);
            char tmp[PATH_MAX];
            const auto newp = vfs::npath_translate(path,tmp);
            return real::__lxstat64(ver,newp,stat_buf);
        }
    }
    __asm__(".symver _iosys_lxstat64,__lxstat64@GLIBC_2.2.5");

    int _iosys_fxstat64(int ver, int fildes, struct stat64 * stat_buf)
    {
        if(vfs::is_image_fd(fildes))
        {
            return vfs::invoke_fs(__VFS(fstat), fildes, *(struct stat*)stat_buf);
        }
        else
        {
            TRACE_SYSCALLN("(%d) -> linux fs", fildes);
            return real::__fxstat64(ver,fildes,stat_buf);
        }
    }
    __asm__(".symver _iosys_fxstat64,__fxstat64@GLIBC_2.2.5");

    int _iosys_fxstatat64(int vers, int fd, const char *filename, struct stat64 *buf, int flag)
    {
        errno = ENOSYS;
        std::cerr << "Unsupported syscall " << __PRETTY_FUNCTION__ << std::endl;
        return -1;
    }
    __asm__(".symver _iosys_fxstatat64,__fxstatat64@GLIBC_2.4");
}
