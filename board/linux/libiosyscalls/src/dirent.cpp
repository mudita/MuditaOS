// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <iosyscalls.hpp>
#include <purefs/vfs_subsystem.hpp>
#include "debug.hpp"

#include <dirent.h>
#include <errno.h>
#include <cstring>
#include <dlfcn.h>
#include <sys/stat.h>


struct __dirstream {
    purefs::fs::filesystem::fsdir dirh;
    struct dirent dir_data;
    size_t position;
};


extern "C" {
    using namespace vfsn::linux::internal;

    DIR *opendir(const char *dirname)
    {
        __dirstream* ret {};
        if(!dirname)
        {
            errno = EINVAL;
        }
        else
        {
            if( redirect_to_image(dirname) )
            {
                auto vfs = purefs::subsystem::vfs_core();
                if(!vfs)
                {
                    errno = EIO;
                }
                else
                {
                    ret = new(std::nothrow)__dirstream;
                    if(!ret)
                    {
                        errno = ENOMEM;
                    }
                    else
                    {
                        ret->position = 0;
                        ret->dirh = vfs->diropen(dirname);
                        if(!ret->dirh)
                        {
                            delete ret;
                            errno = EIO;
                            ret = nullptr;
                        }
                        else if(ret->dirh->error())
                        {
                            errno = -ret->dirh->error();
                            delete ret;
                            ret = nullptr;
                        }
                    }
                }
                if(ret) {
                    add_DIR_to_image_list(ret);
                }
            }
            else
            {
                char tmp[PATH_MAX];
                const auto newpath = npath_translate(dirname,tmp);
                auto r_opendir = reinterpret_cast<DIR* (*)(const char *)>(dlsym(RTLD_NEXT,"opendir"));
                ret = r_opendir(newpath);
            }
        }
        TRACE_SYSCALLN("(%s)=%p errno=%i",dirname,ret,errno);
        return ret;
    }

    __asm__(".symver diropen,diropen@GLIBC_2.2.5");

    int closedir(DIR *dirp)
    {
        int ret {};
        do {
            if (!dirp) {
                errno = EBADF;
                ret = -1;
                break;
            }
            if(is_image_DIR(dirp)) {
                auto vfs = purefs::subsystem::vfs_core();
                if(!vfs) {
                    errno = EIO;
                    ret = -1;
                    break;
                }
                ret = vfs->dirclose(dirp->dirh);
                if(ret < 0) {
                    errno = -ret;
                    ret = -1;
                }
                remove_DIR_from_image_list(dirp);
                delete dirp;
            } else {
                auto r_closedir = reinterpret_cast<int (*)(DIR*)>(dlsym(RTLD_NEXT,"closedir"));
                ret = r_closedir(dirp);
            }
        } while(0);
        TRACE_SYSCALLN("(%p)=%i errno=%i", dirp, ret, errno);
        return ret;
    }
    __asm__(".symver closedir,closedir@GLIBC_2.2.5");

    struct dirent *readdir(DIR *dirp)
    {
        dirent* ret {};
        do {
            if (!dirp) {
                errno = EBADF;
                break;
            }
            if(is_image_DIR(dirp)) {
                auto vfs = purefs::subsystem::vfs_core();
                if(!vfs) {
                    errno = EIO;
                    break;
                }
                std::string fname;
                struct stat stdata;
                auto res = vfs->dirnext(dirp->dirh, fname, stdata);
                if(res < 0) {
                    if(res != -ENODATA) {
                        errno = -res;
                    }
                    break;
                } else {
                    if (fname.size() >= sizeof(dirp->dir_data.d_name)) {
                        errno = EOVERFLOW;
                        break;
                    }
                    dirp->position += 1;
                    dirp->dir_data.d_ino = stdata.st_ino;
                    dirp->dir_data.d_type = S_ISREG(stdata.st_mode)?DT_REG:DT_DIR;
                    dirp->dir_data.d_reclen = fname.size();
                    std::strncpy(dirp->dir_data.d_name,fname.c_str(), sizeof(dirp->dir_data.d_name));
                    ret = &dirp->dir_data;
                }
            } else {
                auto r_readdir = reinterpret_cast<struct dirent* (*)(DIR*)>(dlsym(RTLD_NEXT,"readdir"));
                ret = r_readdir(dirp);
            }
        } while(0);
        TRACE_SYSCALLN("(%p)=%p errno=%i", dirp,ret,errno);
        return ret;
    }
    __asm__(".symver readdir,readdir@GLIBC_2.2.5");

    int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
    {
        TRACE_SYSCALL();
        if (!dirp) {
            errno = EBADF;
            return -1;
        }
        if(is_image_DIR(dirp)) {
            auto vfs = purefs::subsystem::vfs_core();
            if(!vfs) {
                errno = EIO;
                return -1;
            }
            std::string fname;
            struct stat stdata;
            auto res = vfs->dirnext(dirp->dirh, fname, stdata);
            if(res < 0) {
                errno = -res;
                res = -1;
                return res;
            } else {
                if (fname.size() >= sizeof(entry->d_name)) {
                    errno = EOVERFLOW;
                    return -1;
                }
                dirp->position += 1;
                entry->d_ino = stdata.st_ino;
                entry->d_type = S_ISREG(stdata.st_mode)?DT_REG:DT_DIR;
                entry->d_reclen = fname.size();
                std::strncpy(entry->d_name,fname.c_str(), sizeof(entry->d_name));
                *result = entry;
                return 0;
            }
        } else {
            auto r_readdir_r = reinterpret_cast<int (*)(DIR*, struct dirent*, struct dirent**)>(dlsym(RTLD_NEXT,"readdir_r"));
            return r_readdir_r(dirp,entry,result);
        }
    }
    __asm__(".symver readdir_r,readdir_r@GLIBC_2.2.5");

    void rewinddir(DIR *dirp)
    {
        TRACE_SYSCALL();
        if (!dirp) {
            errno = EBADF;
            return;
        }
        if(is_image_DIR(dirp))
        {
            auto vfs = purefs::subsystem::vfs_core();
            if(!vfs) {
                return;
            }
            auto res = vfs->dirreset(dirp->dirh);
            if(res < 0) {
                return;
            }
            dirp->position = 0;
        }
        else
        {
            auto r_rewinddir = reinterpret_cast<void (*)(DIR*)>(dlsym(RTLD_NEXT,"rewinddir"));
            r_rewinddir(dirp);
        }
    }
    __asm__(".symver rewinddir,rewinddir@GLIBC_2.2.5");

    void seekdir( DIR *dirp, long int loc)
    {
        TRACE_SYSCALL();
        if (!dirp) {
            errno = EBADF;
            return;
        }
        if( is_image_DIR(dirp) )
        {
            if (loc < 0) {
                return;
            }
            auto vfs = purefs::subsystem::vfs_core();
            if(!vfs) {
                return;
            }
            if (long(dirp->position) > loc) {
                vfs->dirreset(dirp->dirh);
                dirp->position = 0;
            }
            std::string name;
            struct stat st;
            while ((long(dirp->position) < loc) && (vfs->dirnext(dirp->dirh,name,st))>=0) {
                dirp->position += 1;
            }
        }
        else
        {
            auto r_seekdir = reinterpret_cast<void (*)(DIR*,long int)>(dlsym(RTLD_NEXT,"seekdir"));
            r_seekdir(dirp,loc);
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
        if( is_image_DIR(dirp) )
        {
            return dirp->position;
        }
        else
        {
            auto r_telldir = reinterpret_cast<long int(*)(DIR*)>(dlsym(RTLD_NEXT,"telldir"));
            return r_telldir(dirp);
        }
    }
    __asm__(".symver telldir,telldir@GLIBC_2.2.5");

}

