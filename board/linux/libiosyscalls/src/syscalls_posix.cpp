// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <iosyscalls.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <stdarg.h>
#include <limits.h>

#include "debug.hpp"
#include <purefs/fs/filesystem.hpp>


namespace
{
    int (*real_fprintf)(FILE *__restrict __stream, const char *__restrict __format, ...);

    void __attribute__((constructor)) _lib_posix_initialize()
    {
        real_fprintf = reinterpret_cast<decltype(real_fprintf)>(dlsym(RTLD_NEXT, "fprintf"));
        if(!real_fprintf)
        {
            abort();
        }
    }
}

extern "C" {
    using namespace vfsn::linux::internal;
    int link(const char *oldpath, const char *newpath)
    {
        TRACE_SYSCALL();
        if(redirect_to_image(oldpath))
        {
            errno = ENOSYS;
            real_fprintf(stderr, "Unsupported syscall %s\n", __PRETTY_FUNCTION__ );
            return -1;
        }
        else
        {
            auto r_link = reinterpret_cast<int (*)(const char*,const char*)>(dlsym(RTLD_NEXT,"link"));
            char tmp[PATH_MAX], tmp2[PATH_MAX];
            const auto oldp = npath_translate(oldpath,tmp);
            const auto newp = npath_translate(newpath,tmp2);
            return r_link(oldp,newp);
        }
    }
    __asm__(".symver link,link@GLIBC_2.2.5");

     int unlink(const char *name)
     {
        TRACE_SYSCALL();
        if(redirect_to_image(name))
        {
            return invoke_fs(&purefs::fs::filesystem::unlink, name);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto path = npath_translate(name,tmp);
            auto r_unlink = reinterpret_cast<int (*)(const char*)>(dlsym(RTLD_NEXT,"unlink"));
            return r_unlink(path);
        }
     }
    __asm__(".symver unlink,unlink@GLIBC_2.2.5");

    int stat(const char *file, struct stat *pstat)
    {
        TRACE_SYSCALL();
        if(redirect_to_image(file))
        {
            return invoke_fs(&purefs::fs::filesystem::stat, file, *pstat);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto newfile = npath_translate(file,tmp);
            auto r_xstat = reinterpret_cast<int (*)(int, const char*,struct stat*)>(dlsym(RTLD_NEXT, "__xstat"));
            return r_xstat(1,newfile,pstat);
        }
    }
    __asm__(".symver stat,stat@GLIBC_2.2.5");

    int fstat(int fd, struct stat *pstat)
    {
        TRACE_SYSCALL();
        if(is_image_fd(fd))
        {
            return invoke_fs(&purefs::fs::filesystem::fstat,fd,*pstat);
        }
        else
        {
            auto r_fxstat = reinterpret_cast<int (*)(int, int, struct stat*)>(dlsym(RTLD_NEXT, "__fxstat"));
            return r_fxstat(1,fd,pstat);
        }
    }
    __asm__(".symver fstat,fstat@GLIBC_2.2.5");

    int lstat(const char *pathname, struct stat *statbuf)
    {
        TRACE_SYSCALL();
        if(redirect_to_image(pathname))
        {
            return invoke_fs(&purefs::fs::filesystem::stat, pathname, *statbuf);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto newpath = npath_translate(pathname,tmp);
            auto r_lxstat = reinterpret_cast<int (*)(int, const char*,struct stat*)>(dlsym(RTLD_NEXT, "__lxstat"));
            return r_lxstat(1,newpath,statbuf);
        }
    }
    __asm__(".symver lstat,lstat@GLIBC_2.2.5");

    int fcntl(int fd, int cmd, ... /* arg */ )
    {
        TRACE_SYSCALL();
        if(is_image_fd(fd))
        {
            TRACE_SYSCALL();
            errno = ENOSYS;
            real_fprintf(stderr, "Unsupported syscall %s\n", __PRETTY_FUNCTION__ );
            return -1;
        }
        else
        {
            uintptr_t param;
            va_list args;
            auto r_fcntl = reinterpret_cast<int (*)(int,int,...)>(dlsym(RTLD_NEXT, "fcntl"));
            va_start(args,cmd);
            param = va_arg(args,uintptr_t);
            auto ret =  r_fcntl(fd, cmd, param );
            va_end(args);
            return ret;
        }
    }
    __asm__(".symver fcntl,fcntl@GLIBC_2.2.5");

    int fcntl64(int fd, int cmd, ... /* arg */ )
    {
        TRACE_SYSCALL();
        if(is_image_fd(fd))
        {
            TRACE_SYSCALL();
            real_fprintf(stderr, "Unsupported syscall %s\n", __PRETTY_FUNCTION__ );
            errno = ENOSYS;
            return -1;
        } else {
            uintptr_t param;
            va_list args;
            auto r_fcntl = reinterpret_cast<int (*)(int,int,...)>(dlsym(RTLD_NEXT, "fcntl64"));
            va_start(args,cmd);
            param = va_arg(args,uintptr_t);
            auto ret =  r_fcntl(fd, cmd, param);
            va_end(args);
            return ret;
        }
    }
    __asm__(".symver fcntl64,fcntl64@GLIBC_2.2.5");

    int chdir(const char *path)
    {
        TRACE_SYSCALL();
        if(redirect_to_image(path))
        {
            return invoke_fs(&purefs::fs::filesystem::chdir, path);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto newpath = npath_translate(path,tmp);
            auto r_chdir = reinterpret_cast<int(*)(const char*)>(dlsym(RTLD_NEXT,"chdir"));
            return r_chdir(newpath);
        }
    }
    __asm__(".symver chdir,chdir@GLIBC_2.2.5");

    int fchdir(int fd)
    {
        TRACE_SYSCALL();
        if(is_image_fd(fd))
        {
            errno = ENOSYS;
            real_fprintf(stderr, "Unsupported syscall %s\n", __PRETTY_FUNCTION__ );
            return -1;
        }
        else
        {
            auto r_fchdir = reinterpret_cast<int(*)(int)>(dlsym(RTLD_NEXT,"fchdir"));
            return r_fchdir(fd);
        }
    }
    __asm__(".symver fchdir,fchdir@GLIBC_2.2.5");

    char *getcwd(char *buf, size_t size)
    {
        TRACE_SYSCALL();
        if(redirect_to_image())
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
            auto r_getcwd = reinterpret_cast<char*(*)(char*,size_t)>(dlsym(RTLD_NEXT,"getcwd"));
            return r_getcwd(buf,size);
        }
    }
    __asm__(".symver getcwd,getcwd@GLIBC_2.2.5");

    char *getwd(char *buf)
    {
        TRACE_SYSCALL();
        if(redirect_to_image())
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
            auto r_getwd = reinterpret_cast<char*(*)(char*)>(dlsym(RTLD_NEXT,"getwd"));
            return r_getwd(buf);
        }
    }
    __asm__(".symver getwd,getwd@GLIBC_2.2.5");


    char *get_current_dir_name(void)
    {
        TRACE_SYSCALL();
        if(redirect_to_image())
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
            auto r_getrd = reinterpret_cast<char*(*)()>(dlsym(RTLD_NEXT,"get_current_dir_name"));
            return r_getrd();
        }
    }
    __asm__(".symver get_current_dir_name,get_current_dir_name@GLIBC_2.2.5");


    int rename(const char *oldpath, const char *newpath)
    {
        TRACE_SYSCALL();
        if(redirect_to_image(oldpath))
        {
            return invoke_fs(&purefs::fs::filesystem::rename, oldpath, newpath);
        }
        else
        {
            char tmp[PATH_MAX], tmp2[PATH_MAX];
            const auto oldp = npath_translate(oldpath,tmp);
            const auto newp = npath_translate(newpath,tmp2);
            auto r_rename = reinterpret_cast<int(*)(const char*,const char*)>(dlsym(RTLD_NEXT,"rename"));
            return r_rename(oldp,newp);
        }
    }
    __asm__(".symver rename,rename@GLIBC_2.2.5");

    int mkdir(const char *pathname, mode_t mode)
    {
        if(redirect_to_image(pathname))
        {
            return invoke_fs(&purefs::fs::filesystem::mkdir, pathname, mode);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto path = npath_translate(pathname,tmp);
            auto r_mkdir = reinterpret_cast<int(*)(const char*,mode_t)>(dlsym(RTLD_NEXT,"mkdir"));
            return r_mkdir(path,mode);
        }
    }
    __asm__(".symver mkdir,mkdir@GLIBC_2.2.5");

    int chmod(const char *pathname, mode_t mode)
    {
        TRACE_SYSCALL();
        if(redirect_to_image(pathname))
        {
            return invoke_fs(&purefs::fs::filesystem::chmod,pathname,mode);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto path = npath_translate(pathname,tmp);
            auto r_chmod = reinterpret_cast<int(*)(const char*,mode_t)>(dlsym(RTLD_NEXT,"chmod"));
            return r_chmod(path,mode);
        }
    }
    __asm__(".symver chmod,chmod@GLIBC_2.2.5");

    int fchmod(int fd, mode_t mode)
    {
        TRACE_SYSCALL();
        if(is_image_fd(fd))
        {
            return invoke_fs(&purefs::fs::filesystem::fchmod,fd,mode);
        }
        else
        {
            auto r_fchmod = reinterpret_cast<int(*)(int,mode_t)>(dlsym(RTLD_NEXT,"fchmod"));
            return r_fchmod(fd,mode);
        }
    }
    __asm__(".symver fchmod,fchmod@GLIBC_2.2.5");

    int fsync(int fd)
    {
        TRACE_SYSCALL();
        if(is_image_fd(fd))
        {
            return invoke_fs(&purefs::fs::filesystem::fsync, fd);
        }
        else
        {
            auto r_fsync = reinterpret_cast<int(*)(int)>(dlsym(RTLD_NEXT,"fsync"));
            return r_fsync(fd);
        }
    }
    __asm__(".symver fsync,fsync@GLIBC_2.2.5");

    int fdatasync(int fd)
    {
        TRACE_SYSCALL();
        return fsync(fd);
    }
    __asm__(".symver fdatasync,fdatasync@GLIBC_2.2.5");

    int symlink(const char *target, const char *linkpath)
    {
        TRACE_SYSCALL();
        if(redirect_to_image(target))
        {
            return invoke_fs(&purefs::fs::filesystem::symlink,target,linkpath);
        }
        else
        {
            char tmp[PATH_MAX], tmp2[PATH_MAX];
            const auto tgtp = npath_translate(target,tmp);
            const auto linp = npath_translate(linkpath,tmp2);
            auto r_symlink = reinterpret_cast<int(*)(const char*,const char*)>(dlsym(RTLD_NEXT,"symlink"));
            return r_symlink(tgtp,linp);
        }
    }

    __asm__(".symver symlink,symlink@GLIBC_2.2.5");

    int __symlink(const char *target, const char *linkpath)
    {
        return symlink(target,linkpath);
    }


    int fstatat(int dirfd, const char *pathname, struct stat *statbuf,
            int flags)
    {
        TRACE_SYSCALL();
        errno = ENOSYS;
        real_fprintf(stderr, "Unsupported syscall %s\n", __PRETTY_FUNCTION__ );
        return -1;
    }
    __asm__(".symver fstatat,fstatat@GLIBC_2.2.5");

    int __xstat(int ver, const char * path, struct stat * stat_buf)
    {
        if( redirect_to_image(path) )
        {
            return invoke_fs(&purefs::fs::filesystem::stat, path, *stat_buf);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto newp = npath_translate(path,tmp);
            auto r_xstat = reinterpret_cast<int (*)(int,const char*,struct stat*)>(dlsym(RTLD_NEXT, "__xstat"));
            return r_xstat(ver,newp,stat_buf);
        }
    }

    int __lxstat(int ver, const char * path, struct stat * stat_buf)
    {
        if( redirect_to_image(path) )
        {
            return invoke_fs(&purefs::fs::filesystem::stat, path, *stat_buf);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto newp = npath_translate(path,tmp);
            auto r_xstat = reinterpret_cast<int (*)(int,const char*,struct stat*)>(dlsym(RTLD_NEXT, "__lxstat"));
            return r_xstat(ver,newp,stat_buf);
        }
    }

    int __fxstat(int ver, int fildes, struct stat * stat_buf)
    {
        if( is_image_fd(fildes) )
        {
            return invoke_fs(&purefs::fs::filesystem::fstat, fildes, *stat_buf);
        }
        else
        {
            auto r_fxstat = reinterpret_cast<int (*)(int,int,struct stat*)>(dlsym(RTLD_NEXT, "__fxstat"));
            return r_fxstat(ver,fildes,stat_buf);
        }
    }
}
