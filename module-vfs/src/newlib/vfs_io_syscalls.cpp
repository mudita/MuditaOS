// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <vfs.hpp>
#include <errno.h>
#include <dirent.h>
#include <log/log.hpp>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include "vfs_internal_dirent.hpp"
#include "HandleManager.hpp"
#include <newlib/vfs_io_syscalls.hpp>

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
            _errno_ = EMFILE;
            LOG_ERROR("Unable to find handle %i", hwnd);
            return -1;
        }
        else {
            _errno_ = 0;
            return hwnd;
        }
    }

    long close(int &_errno_, int fd)
    {
        auto fil = gFileHandles.clearHandle(fd);
        if (!fil) {
            LOG_ERROR("Unable to find handle %i", fd);
            _errno_ = EBADF;
            return -1;
        }
        auto ret = ff_fclose(fil);
        _errno_  = stdioGET_ERRNO();
        return ret;
    }

    long write(int &_errno_, int fd, const void *buf, size_t cnt)
    {
        if (fd == STDERR_FILENO || fd == STDOUT_FILENO) {
            log_WriteToDevice((uint8_t *)buf, cnt);
            return cnt;
        }
        auto fil = gFileHandles[fd];
        if (!fil) {
            LOG_ERROR("Unable to find handle %i", fd);
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
            LOG_ERROR("Unable to find handle %i", fd);
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
            LOG_ERROR("Unable to find handle %i", fd);
            _errno_ = EBADF;
            return -1;
        }
        auto ret = ff_fseek(fil, pos, dir);
        _errno_  = stdioGET_ERRNO();
        return ret;
    }
    int fstat(int &_errno_, int fd, struct stat *pstat)
    {
        auto fil = gFileHandles[fd];
        if (!fil) {
            LOG_ERROR("Unable to find handle %i", fd);
            _errno_ = EBADF;
            return -1;
        }
        std::memset(pstat, 0, sizeof(*pstat));
        pstat->st_ino  = fil->ulObjectCluster;
        pstat->st_size = fil->ulFileSize;
        pstat->st_dev  = 0;
        pstat->st_mode = S_IFREG | 0666;
        _errno_        = 0;
        return 0;
    }
    int link(int &_errno_, const char *existing, const char *newLink)
    {
        _errno_ = ENOSYS;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int unlink(int &_errno_, const char *name)
    {
        auto ret = ff_remove(name);
        if (ret && stdioGET_ERRNO() == EISDIR)
            ret = ff_deltree(name, nullptr, nullptr);
        _errno_ = stdioGET_ERRNO();
        return ret;
    }

    int fcntl(int &_errno_, int fd, int cmd, int arg)
    {
        _errno_ = ENOSYS;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int stat(int &_errno_, const char *file, struct stat *pstat)
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
        _errno_ = stdioGET_ERRNO();
        // NOTE: ff_stdio uses wrong errno NOADDRESS
        if (_errno_ == EFAULT)
            _errno_ = ENOENT;
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
        auto dir      = new DIR;
        dir->dir_data = dirent::diropen(_errno_, dirname);
        if (!dir->dir_data) {
            delete dir;
            return nullptr;
        }
        dir->position            = 0;
        dir->file_data.d_ino     = -1;
        dir->file_data.d_name[0] = '\0';
        return dir;
    }

    int closedir(int &_errno_, DIR *dirp)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return -1;
        }
        auto res = dirent::dirclose(_errno_, dirp->dir_data);
        delete dirp;
        return res;
    }

    struct dirent *readdir(int &_errno_, DIR *dirp)
    {
        if (!dirp) {
            _errno_ = EBADF;
            return nullptr;
        }
        auto olderrno{_errno_};
        auto res = dirent::dirnext(_errno_, dirp->dir_data);
        auto fff = reinterpret_cast<FF_FindData_t *>(dirp->dir_data->dir_state);
        if (res < 0) {
            if (_errno_ == pdFREERTOS_ERRNO_ENMFILE) {
                _errno_ = olderrno;
            }
            return nullptr;
        }
        dirp->position += 1;
        if (strnlen(fff->pcFileName, NAME_MAX) >= sizeof(dirp->file_data.d_name)) {
            _errno_ = EOVERFLOW;
            return nullptr;
        }
        dirp->file_data.d_ino  = fff->xDirectoryEntry.ulObjectCluster;
        dirp->file_data.d_type = (fff->ucAttributes & FF_FAT_ATTR_DIR) ? DT_DIR : DT_REG;
        std::strncpy(dirp->file_data.d_name, fff->pcFileName, sizeof(dirp->file_data.d_name));
        return &dirp->file_data;
    }

    int readdir_r(int &_errno_, DIR *dirp, struct dirent *entry, struct dirent **result)
    {
        if (!dirp) {
            return EBADF;
        }
        auto olderrno{_errno_};
        auto res = dirent::dirnext(_errno_, dirp->dir_data);
        auto fff = reinterpret_cast<FF_FindData_t *>(dirp->dir_data->dir_state);
        if (res < 0) {
            res = _errno_;
            if (_errno_ == pdFREERTOS_ERRNO_ENMFILE) {
                res = 0;
            }
            _errno_ = olderrno;
            return res;
        }
        dirp->position += 1;
        if (strnlen(fff->pcFileName, NAME_MAX) >= sizeof(entry->d_name)) {
            return EOVERFLOW;
        }
        entry->d_ino  = fff->xDirectoryEntry.ulObjectCluster;
        entry->d_type = (fff->ucAttributes & FF_FAT_ATTR_DIR) ? DT_DIR : DT_REG;
        std::strncpy(entry->d_name, fff->pcFileName, sizeof(entry->d_name));
        *result = entry;
        return 0;
    }

    void rewinddir(int &_errno_, DIR *dirp)
    {
        if (!dirp) {
            return;
        }
        dirent::dirreset(_errno_, dirp->dir_data);
        dirp->position = 0;
    }

    void seekdir(int &_errno_, DIR *dirp, long int loc)
    {
        if (!dirp || loc < 0) {
            return;
        }
        if (dirp->position > loc) {
            dirent::dirreset(_errno_, dirp->dir_data);
            dirp->position = 0;
        }
        while ((dirp->position < loc) && (dirent::dirnext(_errno_, dirp->dir_data) >= 0)) {
            dirp->position += 1;
        }
    }
    long int telldir(int &_errno_, DIR *dirp)
    {
        if (!dirp) {
            return -1;
        }
        return dirp->position;
    }

    int chmod(int &_errno_, const char *path, mode_t mode)
    {
        _errno_ = ENOSYS;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int fchmod(int &_errno_, int fd, mode_t mode)
    {
        _errno_ = ENOSYS;
        LOG_ERROR("Syscall %s not supported", __PRETTY_FUNCTION__);
        return -1;
    }
    int fsync(int &_errno_, int fd)
    {
        auto fil = gFileHandles[fd];
        if (!fil) {
            LOG_ERROR("Unable to find handle %i", fd);
            _errno_ = EBADF;
            return -1;
        }
        auto ret = ff_fflush(fil);
        _errno_  = stdioGET_ERRNO();
        return ret;
    }
} // namespace vfsn::internal::syscalls
