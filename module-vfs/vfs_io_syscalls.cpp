// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <vfs.hpp>
#include "vfs_io_syscalls.hpp"
#include <errno.h>
#include <dirent.h>
#include <log/log.hpp>
#include <fcntl.h>
#include <cstring>
#include "HandleManager.hpp"

/** NOTE: This is generic wrapper for newlib syscalls
 * using current VFS implementation. This implementation
 * will be removed in next release when disk manager
 * and new VFS filesystem will be ready
 */

namespace vfsn::internal
{
    namespace
    {
        stdlib::HandleManager<FF_FILE> gFileHandles;
    }

    namespace
    {
        auto open_to_fopen_flags(int flags)
        {
            if (flags & O_RDONLY) {
                return "r";
            }
            else if (flags & (O_WRONLY | O_CREAT | O_TRUNC)) {
                return "w";
            }
            else if (flags & (O_WRONLY | O_CREAT | O_APPEND)) {
                return "a";
            }
            else if (flags & O_RDWR) {
                return "r+";
            }
            else if (flags & (O_RDWR | O_CREAT | O_TRUNC)) {
                return "w+";
            }
            else if (flags & (O_RDWR | O_CREAT | O_APPEND)) {
                return "a+";
            }
            else {
                LOG_ERROR("Unsupported flag %08x", flags);
                return "";
            }
        }
    } // namespace
    struct internal_dir_state
    {
        internal_dir_state() : ff_data(std::make_unique<FF_FindData_t>())
        {}
        std::string base_name;
        std::unique_ptr<FF_FindData_t> ff_data;
    };
} // namespace vfsn::internal

namespace vfsn::internal::syscalls
{
    int open(int &_errno_, const char *file, int flags, int mode)
    {
        auto fflags = open_to_fopen_flags(flags);
        if (*fflags == '\0') {
            _errno_ = EINVAL;
            return -1;
        }
        auto fil = ff_fopen(file, fflags);
        if (!fil) {
            _errno_ = stdioGET_ERRNO();
            return -1;
        }
        int hwnd = gFileHandles.setHandle(fil);
        if (hwnd < 0) {
            ff_fclose(fil);
            _errno_ = hwnd;
            LOG_ERROR("Unable to find handle");
            return -1;
        }
        else {
            _errno_ = 0;
            return hwnd;
        }
    }

    long close(int &_errno_, int fd)
    {
        auto fil = gFileHandles[fd];
        if (!fil) {
            LOG_ERROR("Unable to find handle");
            _errno_ = EBADF;
            return -1;
        }
        auto ret = ff_fclose(fil);
        _errno_  = stdioGET_ERRNO();
        return ret;
    }

    long write(int &_errno_, int fd, const void *buf, size_t cnt)
    {
        auto fil = gFileHandles[fd];
        if (!fil) {
            LOG_ERROR("Unable to find handle");
            _errno_ = EBADF;
            return -1;
        }
        auto ret = ff_fwrite(buf, cnt, 1, fil);
        _errno_  = stdioGET_ERRNO();
        return ret * cnt;
    }

    long read(int &_errno_, int fd, void *buf, size_t cnt)
    {
        auto fil = gFileHandles[fd];
        if (!fil) {
            LOG_ERROR("Unable to find handle");
            _errno_ = EBADF;
            return -1;
        }
        auto ret = ff_fread(buf, cnt, 1, fil);
        _errno_  = stdioGET_ERRNO();
        return ret * cnt;
    }

    off_t lseek(int &_errno_, int fd, off_t pos, int dir)
    {
        auto fil = gFileHandles[fd];
        if (!fil) {
            LOG_ERROR("Unable to find handle");
            _errno_ = EBADF;
            return -1;
        }
        auto ret = ff_fseek(fil, pos, dir);
        _errno_  = stdioGET_ERRNO();
        return ret;
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
        auto ret = ff_remove(name);
        _errno_  = stdioGET_ERRNO();
        return ret;
    }

    int fcntl(int &_errno_, int fd, int cmd, int arg)
    {
        _errno_ = ENOTSUP;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int stat_r(int &_errno_, const char *file, struct stat *pstat)
    {
        FF_Stat_t stat_ff;
        auto ret = ff_stat(file, &stat_ff);
        if (!ret) {
            std::memset(pstat, 0, sizeof(*pstat));
            pstat->st_ino  = stat_ff.st_ino;
            pstat->st_size = stat_ff.st_size;
            pstat->st_dev  = stat_ff.st_dev;
            pstat->st_mode = stat_ff.st_mode;
        }
        _errno_ = stdioGET_ERRNO();
        return ret;
    }
    int chdir(int &_errno_, const char *path)
    {
        auto ret = ff_chdir(path);
        _errno_  = stdioGET_ERRNO();
        return ret;
    }
    char *getcwd(int &_errno_, char *buf, size_t size)
    {
        auto cwd = ff_getcwd(buf, size);
        _errno_  = stdioGET_ERRNO();
        return cwd;
    }
    int rename(int &_errno_, const char *oldName, const char *newName)
    {
        auto ret = ff_rename(oldName, newName, true);
        _errno_  = stdioGET_ERRNO();
        return ret;
    }

    int mkdir(int &_errno_, const char *path, uint32_t mode)
    {
        auto ret = ff_mkdir(path);
        _errno_  = stdioGET_ERRNO();
        return ret;
    }

    DIR *opendir(int &_errno_, const char *dirname)
    {
        auto dir                 = new DIR;
        dir->dir_data            = new DIR_ITER;
        dir->position            = 0;
        auto ff_find             = new internal_dir_state();
        dir->dir_data->dir_state = ff_find;
        ff_find->base_name       = dirname;
        auto ret                 = ff_findfirst(dirname, ff_find->ff_data.get());
        if (ret < 0) {
            delete reinterpret_cast<internal_dir_state *>(dir->dir_data->dir_state);
            delete dir->dir_data;
            delete dir;
        }
        _errno_ = stdioGET_ERRNO();
        return dir;
    }
    int closedir(int &_errno_, DIR *dirp)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return -1;
        }
        delete reinterpret_cast<internal_dir_state *>(dirp->dir_data->dir_state);
        delete dirp->dir_data;
        delete dirp;
        _errno_ = 0;
        return 0;
    }

    struct dirent *readdir(int &_errno_, DIR *dirp)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return nullptr;
        }
        auto ret     = &dirp->file_data;
        auto ff_find = reinterpret_cast<internal_dir_state *>(dirp->dir_data->dir_state)->ff_data.get();
        ret->d_ino   = ff_find->xDirectoryEntry.ulObjectCluster;
        ret->d_type  = (ff_find->ucAttributes & FF_FAT_ATTR_DIR) ? DT_DIR : DT_REG;
        std::strncpy(ret->d_name, ff_find->pcFileName, sizeof(ret->d_name));
        _errno_ = 0;
        return ret;
    }
    int readdir_r(int &_errno_, DIR *dirp, struct dirent *entry, struct dirent **result)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return -1;
        }
        *result      = new dirent;
        auto ret     = *result;
        auto ff_find = reinterpret_cast<internal_dir_state *>(dirp->dir_data->dir_state)->ff_data.get();
        ret->d_ino   = ff_find->xDirectoryEntry.ulObjectCluster;
        ret->d_type  = (ff_find->ucAttributes & FF_FAT_ATTR_DIR) ? DT_DIR : DT_REG;
        std::strncpy(ret->d_name, ff_find->pcFileName, sizeof(ret->d_name));
        _errno_ = 0;
        return 0;
    }

    void rewinddir(int &_errno_, DIR *dirp)
    {
        dirp->position = 0;
        auto int_state = reinterpret_cast<internal_dir_state *>(dirp->dir_data->dir_state);
        ff_findfirst(int_state->base_name.c_str(), int_state->ff_data.get());
        _errno_ = stdioGET_ERRNO();
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
        auto fil = gFileHandles[fd];
        if (!fil) {
            LOG_ERROR("Unable to find handle");
            _errno_ = EBADF;
            return -1;
        }
        auto ret = ff_fflush(fil);
        _errno_  = stdioGET_ERRNO();
        return ret;
    }
} // namespace vfsn::internal::syscalls