// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "internal.hpp"
#include <ff_stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>
#include <stdio.h>
#include <cstring>
#include "internal.hpp"

namespace
{
    int (*real_fprintf)(FILE *__restrict __stream, const char *__restrict __format, ...);

    void __attribute__((constructor)) _lib_posix_initialize()
    {
        real_fprintf = reinterpret_cast<decltype(real_fprintf)>(dlsym(RTLD_NEXT, "fprintf"));
        if(!real_fprintf ) {
            abort();
        }
    }
}
extern "C" {

	int link(const char *oldpath, const char *newpath)
	{
		errno = ENOSYS;
		real_fprintf(stderr, "Unsupported syscall %s\n", __PRETTY_FUNCTION__ );
		return -1;
	}
    __asm__(".symver link,link@GLIBC_2.2.5");

	 int unlink(const char *name)
	 {
		auto ret = ff_remove(name);
        if (ret && stdioGET_ERRNO() == EISDIR)
            ret = ff_deltree(name, nullptr, nullptr);
        errno = stdioGET_ERRNO();
        return ret;
	 }
    __asm__(".symver unlink,unlink@GLIBC_2.2.5");

	int stat(const char *file, struct stat *pstat)
	{
        FF_Stat_t stat_ff;
        auto ret = ff_stat(file, &stat_ff);
        if (!ret) {
            std::memset(pstat, 0, sizeof(*pstat));
            pstat->st_ino  = stat_ff.st_ino;
            pstat->st_size = stat_ff.st_size;
            pstat->st_dev  = stat_ff.st_dev;
            // Linux mode compat
            if (stat_ff.st_mode == FF_IFREG)
                pstat->st_mode = S_IFREG | 0666;
            if (stat_ff.st_mode == FF_IFDIR)
                pstat->st_mode = S_IFDIR | 0777;
        }
        errno = stdioGET_ERRNO();
        // NOTE: ff_stdio uses wrong errno NOADDRESS
        if (errno == EFAULT)
            errno = ENOENT;
        return ret;
	}
    __asm__(".symver stat,stat@GLIBC_2.2.5");
    int fstat(int fd, struct stat *pstat)
	{
		FF_FILE* fil = vfsn::linux::internal::handle_to_ff_file(fd);
        if(!fil) {
            errno = EBADF;
            return -1;
        }
        std::memset(pstat, 0, sizeof(*pstat));
        pstat->st_ino  = fil->ulObjectCluster;
        pstat->st_size = fil->ulFileSize;
        pstat->st_dev  = 0;
        pstat->st_mode = S_IFREG | 0666;
        errno        = 0;
        return 0;
	}
    __asm__(".symver fstat,fstat@GLIBC_2.2.5");

    int lstat(const char *pathname, struct stat *statbuf)
	{
		return stat(pathname, statbuf);
	}
    __asm__(".symver lstat,lstat@GLIBC_2.2.5");

    int fcntl(int fd, int cmd, ... /* arg */ )
    {
        errno = ENOSYS;
        real_fprintf(stderr, "Unsupported syscall %s\n", __PRETTY_FUNCTION__ );
        return -1;
    }
    __asm__(".symver fcntl,fcntl@GLIBC_2.2.5");

    int chdir(const char *path)
    {
        auto ret = ff_chdir(path);
        errno  = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver chdir,chdir@GLIBC_2.2.5");
    int fchdir(int fd)
    {
        errno = ENOSYS;
        real_fprintf(stderr, "Unsupported syscall %s\n", __PRETTY_FUNCTION__ );
        return -1;
    }
    __asm__(".symver fchdir,fchdir@GLIBC_2.2.5");

    char *getcwd(char *buf, size_t size)
    {
        auto cwd = ff_getcwd(buf, size);
        errno  = stdioGET_ERRNO();
        return cwd;
    }
    __asm__(".symver getcwd,getcwd@GLIBC_2.2.5");

    char *getwd(char *buf)
    {
        auto cwd = ff_getcwd(buf, ffconfigMAX_FILENAME );
        errno  = stdioGET_ERRNO();
        return cwd;
    }
    __asm__(".symver getwd,getwd@GLIBC_2.2.5");


    char *get_current_dir_name(void)
    {
        auto buf = new char[ffconfigMAX_FILENAME];
        auto cwd = ff_getcwd(buf, ffconfigMAX_FILENAME );
        errno  = stdioGET_ERRNO();
        return cwd;
    }
    __asm__(".symver get_current_dir_name,get_current_dir_name@GLIBC_2.2.5");


    int rename(const char *oldpath, const char *newpath)
    {
        auto ret = ff_rename(oldpath, newpath, true);
        errno  = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver rename,rename@GLIBC_2.2.5");

    int mkdir(const char *pathname, mode_t )
    {
        auto ret = ff_mkdir(pathname);
        errno  = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver mkdir,mkdir@GLIBC_2.2.5");

    int chmod(const char *pathname, mode_t mode)
    {
        errno = ENOSYS;
        real_fprintf(stderr, "Unsupported syscall %s\n", __PRETTY_FUNCTION__ );
        return -1;
    }
    __asm__(".symver chmod,chmod@GLIBC_2.2.5");

    int fchmod(int fd, mode_t mode)
    {
        errno = ENOSYS;
        real_fprintf(stderr, "Unsupported syscall %s\n", __PRETTY_FUNCTION__ );
        return -1;
    }
    __asm__(".symver fchmod,fchmod@GLIBC_2.2.5");

    int fsync(int fd)
    {
        auto fil = vfsn::linux::internal::handle_to_ff_file(fd);
        if (!fil) {
            LOG_ERROR("Unable to find handle %i", fd);
            errno = EBADF;
            return -1;
        }
        auto ret = ff_fflush(fil);
        errno  = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fsync,fsync@GLIBC_2.2.5");

    int fdatasync(int fd)
    {
        return fsync(fd);
    }
    __asm__(".symver fdatasync,fdatasync@GLIBC_2.2.5");

}
