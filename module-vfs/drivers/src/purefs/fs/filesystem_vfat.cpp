// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/drivers/filesystem_vfat.hpp>
#include <purefs/fs/drivers/mount_point_vfat.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/fs/drivers/file_handle_vfat.hpp>
#include <purefs/fs/drivers/directory_handle_vfat.hpp>
#include <purefs/fs/mount_flags.hpp>
#include <log/log.hpp>
#include <fatfs/volume_mapper.hpp>
#include <ff.h>
#include <sys/statvfs.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace purefs::fs::drivers
{
    namespace
    {
        int translate_error(int error)
        {
            switch (error) {
            case FR_OK:
                return 0;
            case FR_NO_FILE:
            case FR_NO_PATH:
            case FR_INVALID_NAME:
                return -ENOENT;
            case FR_DENIED:
                return -EACCES;
            case FR_EXIST:
                return -EEXIST;
            case FR_INVALID_OBJECT:
                return -EBADF;
            case FR_WRITE_PROTECTED:
                return -EROFS;
            case FR_INVALID_DRIVE:
            case FR_NOT_ENABLED:
            case FR_NO_FILESYSTEM:
                return -ENODEV;
            case FR_NOT_ENOUGH_CORE:
                return -ENOMEM;
            case FR_TOO_MANY_OPEN_FILES:
                return -EMFILE;
            case FR_INVALID_PARAMETER:
                return -EINVAL;
            case FR_LOCKED:
            case FR_TIMEOUT:
            case FR_MKFS_ABORTED:
            case FR_DISK_ERR:
            case FR_INT_ERR:
            case FR_NOT_READY:
                return -EIO;
            }

            return -EIO;
        }

        uint8_t translate_flags(unsigned flags)
        {
            uint8_t fat_mode = 0;
            switch (flags & O_ACCMODE) {
            case O_RDONLY:
                fat_mode |= FA_READ;
                break;
            case O_WRONLY:
                fat_mode |= FA_WRITE;
                break;
            case O_RDWR:
                fat_mode |= (FA_READ | FA_WRITE);
                break;
            }
            if (flags & O_APPEND)
                fat_mode |= FA_OPEN_APPEND;
            if (flags & O_CREAT)
                fat_mode |= FA_OPEN_ALWAYS;
            if (flags & O_TRUNC)
                fat_mode |= FA_CREATE_ALWAYS;
            return fat_mode;
        }

        auto translate_fat_attrib_to_mode(BYTE fattrib, bool ro)
        {
            decltype(static_cast<struct stat *>(nullptr)->st_mode) mode = S_IRUSR | S_IRGRP | S_IROTH;

            if (fattrib & AM_DIR) {
                mode |= (S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH);
            }
            else {
                mode |= S_IFREG;
            }
            if ((fattrib & AM_RDO) == 0 && !ro) {
                mode |= (S_IWUSR | S_IWGRP | S_IWOTH);
            }
            if (fattrib & AM_HID) {}
            if (fattrib & AM_SYS) {}
            if (fattrib & AM_ARC) {}
            return mode;
        }

        auto translate_mode_to_attrib(decltype(static_cast<struct stat *>(nullptr)->st_mode) mode)
        {
            BYTE attr{0};
            if ((mode & (S_IWGRP | S_IWUSR | S_IWOTH)) == 0)
                attr |= AM_RDO;
            return attr;
        }

        void translate_filinfo_to_stat(const FILINFO &fs, const FIL *fil, bool ro, struct stat &st)
        {
            if (fil) {
                st.st_dev = fil->obj.id;
                st.st_ino = fil->obj.sclust;
            }
            else {
                st.st_dev = 0;
                st.st_ino = 0;
            }
            st.st_mode  = translate_fat_attrib_to_mode(fs.fattrib, ro);
            st.st_nlink = 1;
            st.st_uid   = 0;
            st.st_gid   = 0;
            st.st_rdev  = 0;
            st.st_size  = fs.fsize;

            // TODO: Block FF_MIN_SS != FF_MAX_SS
#if FF_MAX_SS != FF_MIN_SS
            st.st_blksize = fatfs->ssize;
#else
            st.st_blksize = FF_MIN_SS;
#endif
            // TODO: Time is currently not supported
            st.st_blocks = fs.fsize / st.st_blksize;
            st.st_atime  = 0;
            st.st_mtime  = 0;
            st.st_ctime  = 0;
        }
    } // namespace

    auto filesystem_vfat::mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                                         std::string_view path,
                                         unsigned flags) -> fsmount
    {
        return std::make_shared<mount_point_vfat>(diskh, path, flags, shared_from_this());
    }

    auto filesystem_vfat::mount(fsmount mnt, const void *data) noexcept -> int
    {
        auto disk = mnt->disk();
        if (!disk) {
            return -EIO;
        }
        auto ret = ffat::internal::append_volume(disk);
        if (ret < 0) {
            LOG_ERROR("Unable to attach volume to ff layer with errno %i", ret);
            return ret;
        }
        auto vmnt = std::dynamic_pointer_cast<mount_point_vfat>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non VFAT mount point");
            return -EIO;
        }
        vmnt->ff_drive(ret);
        ret = f_mount(vmnt->fatfs(), vmnt->ff_drive(), 1);
        ret = translate_error(ret);
        if (!ret) {
            filesystem_operations::mount(mnt, data);
        }
        return ret;
    }

    auto filesystem_vfat::filesystem_register_completed() const noexcept -> int
    {
        const auto dmgr = disk_mngr();
        if (dmgr) {
            ffat::internal::reset_volumes(dmgr);
            return 0;
        }
        else {
            return -EIO;
        }
    }

    auto filesystem_vfat::umount(fsmount mnt) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_vfat>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non VFAT mount point");
            return -EIO;
        }
        const auto lun = vmnt->ff_lun();
        if (lun < 0) {
            LOG_ERROR("Wrong ff_lun");
            return -ENXIO;
        }
        auto disk = mnt->disk();
        if (!disk) {
            return -EIO;
        }
        int ret = f_unmount(vmnt->ff_drive());
        ret     = translate_error(ret);
        if (!ret) {
            ret = ffat::internal::remove_volume(disk);
            filesystem_operations::umount(mnt);
        }
        return ret;
    }

    auto filesystem_vfat::stat_vfs(fsmount mnt, std::string_view, struct statvfs &stat) const noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_vfat>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non VFAT mount point");
            return -EIO;
        }
        DWORD xfree;
        FATFS *fatfs;
        const auto res = f_getfree(vmnt->ff_drive(), &xfree, &fatfs);
        if (res != FR_OK) {
            return -EIO;
        }
        /*
         * _MIN_SS holds the sector size. It is one of the configuration
         * constants used by the FS module
         */
        stat.f_bsize = FF_MIN_SS;
#if FF_MAX_SS != FF_MIN_SS
        stat.f_frsize = fatfs->csize * fatfs->ssize;
#else
        stat.f_frsize = fatfs->csize * FF_MIN_SS;
#endif
        stat.f_blocks  = (fatfs->n_fatent - 2);
        stat.f_bfree   = xfree;
        stat.f_bavail  = xfree;
        stat.f_flag    = vmnt->flags();
        stat.f_files   = 0;
        stat.f_ffree   = 0;
        stat.f_favail  = 0;
        stat.f_fsid    = 0;
        stat.f_namemax = PATH_MAX;
        return translate_error(res);
    }

    auto filesystem_vfat::open(fsmount mnt, std::string_view path, int flags, int mode) noexcept -> fsfile
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_vfat>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non VFAT mount point");
            return nullptr;
        }
        const auto fspath  = vmnt->native_path(path);
        const auto fsflags = translate_flags(flags);
        auto fileo         = std::make_shared<file_handle_vfat>(mnt, fspath, flags);
        auto ret           = f_open(fileo->ff_filp(), fspath.c_str(), fsflags);
        fileo->error(translate_error(ret));
        return fileo;
    }

    auto filesystem_vfat::close(fsfile zfile) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_vfat>(zfile);
        if (!vfile) {
            LOG_ERROR("Non fat filesystem pointer");
            return -EBADF;
        }
        const auto fres = f_close(vfile->ff_filp());
        return translate_error(fres);
    }

    auto filesystem_vfat::write(fsfile zfile, const char *ptr, size_t len) noexcept -> ssize_t
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_vfat>(zfile);
        if (!vfile) {
            LOG_ERROR("Non fat filesystem pointer");
            return -EBADF;
        }
        UINT bytes_written;
        const auto fres = f_write(vfile->ff_filp(), ptr, len, &bytes_written);
        return (fres == FR_OK) ? (bytes_written) : translate_error(fres);
    }

    auto filesystem_vfat::read(fsfile zfile, char *ptr, size_t len) noexcept -> ssize_t
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_vfat>(zfile);
        if (!vfile) {
            LOG_ERROR("Non fat filesystem pointer");
            return -EBADF;
        }
        UINT bytes_read;
        const auto fres = f_read(vfile->ff_filp(), ptr, len, &bytes_read);
        return (fres == FR_OK) ? (bytes_read) : translate_error(fres);
    }

    auto filesystem_vfat::seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_vfat>(zfile);
        if (!vfile) {
            LOG_ERROR("Non fat filesystem pointer");
            return -EBADF;
        }
        off_t cpos{0};
        auto fp = vfile->ff_filp();
        if (f_error(fp) != FR_OK) {
            return translate_error(f_error(fp));
        }
        if (dir == SEEK_CUR) {
            cpos = f_tell(fp);
        }
        else if (dir == SEEK_SET) {
            cpos = 0;
        }
        else if (dir == SEEK_END) {
            cpos = f_size(fp);
        }
        else {
            return -EINVAL;
        }
        const auto newpos = cpos + pos;
        if (newpos < 0) {
            return -ENXIO;
        }
        FRESULT fres{FR_OK};
        if (f_tell(fp) != static_cast<unsigned long>(newpos)) {
            fres = f_lseek(fp, newpos);
        }
        return (fres == FR_OK) ? (f_tell(fp)) : translate_error(fres);
    }

    auto filesystem_vfat::fstat(fsfile zfile, struct stat &st) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_vfat>(zfile);
        if (!vfile) {
            LOG_ERROR("Non fat filesystem pointer");
            return -EBADF;
        }
        FILINFO finfo;
        const auto fres = f_stat(vfile->open_path().c_str(), &finfo);
        if (fres == FR_OK) {
            const auto vmnt = vfile->mntpoint();
            if (vmnt) {
                translate_filinfo_to_stat(finfo, vfile->ff_filp(), vmnt->is_ro(), st);
            }
            else {
                return -EIO;
            }
        }
        return translate_error(fres);
    }

    auto filesystem_vfat::stat(fsmount mnt, std::string_view file, struct stat &st) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_vfat>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non VFAT mount point");
            return -EBADF;
        }
        FILINFO finfo;
        const auto fspath = vmnt->native_path(file);
        const int fres    = f_stat(fspath.c_str(), &finfo);
        if (fres == FR_OK) {
            translate_filinfo_to_stat(finfo, nullptr, vmnt->is_ro(), st);
        }
        return translate_error(fres);
    }

    auto filesystem_vfat::unlink(fsmount mnt, std::string_view name) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_vfat>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non VFAT mount point");
            return -ENXIO;
        }
        const auto fspath = vmnt->native_path(name);
        const auto fret   = f_unlink(fspath.c_str());
        return translate_error(fret);
    }

    auto filesystem_vfat::rename(fsmount mnt, std::string_view oldname, std::string_view newname) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_vfat>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non VFAT mount point");
            return -ENXIO;
        }
        const auto fsold = vmnt->native_path(oldname);
        const auto fsnew = vmnt->native_path(newname);
        const auto fret  = f_rename(fsold.c_str(), fsnew.c_str());
        return translate_error(fret);
    }

    auto filesystem_vfat::mkdir(fsmount mnt, std::string_view path, int mode) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_vfat>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non VFAT mount point");
            return -ENXIO;
        }
        const auto fspath = vmnt->native_path(path);
        const auto fret   = f_mkdir(fspath.c_str());
        return translate_error(fret);
    }

    auto filesystem_vfat::diropen(fsmount mnt, std::string_view path) noexcept -> fsdir
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_vfat>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non VFAT mount point");
            return nullptr;
        }
        const auto fspath = vmnt->native_path(path);
        const auto dirp   = std::make_shared<directory_handle_vfat>(mnt, 0);
        const auto fret   = f_opendir(dirp->ff_dirp(), fspath.c_str());
        dirp->error(translate_error(fret));
        return dirp;
    }

    auto filesystem_vfat::dirreset(fsdir dirstate) noexcept -> int
    {
        auto dirp = std::dynamic_pointer_cast<directory_handle_vfat>(dirstate);
        if (!dirp) {
            LOG_ERROR("Non VFAT directory handle");
            return -ENXIO;
        }
        const auto ferr = f_rewinddir(dirp->ff_dirp());
        return translate_error(ferr);
    }

    auto filesystem_vfat::dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) -> int
    {
        auto dirp = std::dynamic_pointer_cast<directory_handle_vfat>(dirstate);
        if (!dirp) {
            LOG_ERROR("Non VFAT directory handle");
            return -ENXIO;
        }
        FILINFO ffinfo;
        const auto ferr = f_readdir(dirp->ff_dirp(), &ffinfo);
        if (ferr == FR_OK) {
            if (ffinfo.fname[0] == '\0') {
                return -ENODATA;
            }
            else {
                const auto vmnt = dirp->mntpoint();
                if (vmnt) {
                    translate_filinfo_to_stat(ffinfo, nullptr, vmnt->is_ro(), filestat);
                    filename = ffinfo.fname;
                }
                else {
                    return -EIO;
                }
            }
        }
        return translate_error(ferr);
    }

    auto filesystem_vfat::dirclose(fsdir dirstate) noexcept -> int
    {
        auto dirp = std::dynamic_pointer_cast<directory_handle_vfat>(dirstate);
        if (!dirp) {
            LOG_ERROR("Not a vfat directory handle");
            return -ENXIO;
        }
        const auto ferr = f_closedir(dirp->ff_dirp());
        return translate_error(ferr);
    }

    auto filesystem_vfat::ftruncate(fsfile zfile, off_t len) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_vfat>(zfile);
        if (!vfile) {
            LOG_ERROR("Non fat filesystem pointer");
            return -EBADF;
        }
        const int fres = f_truncate(vfile->ff_filp());
        return translate_error(fres);
    }

    auto filesystem_vfat::fsync(fsfile zfile) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_vfat>(zfile);
        if (!vfile) {
            LOG_ERROR("Non fat filesystem pointer");
            return -EBADF;
        }
        const int fres = f_sync(vfile->ff_filp());
        return translate_error(fres);
    }

    auto filesystem_vfat::isatty(fsfile zfile) noexcept -> int
    {
        // NOTE: Handle fvat is always not a tty
        return 0;
    }

    auto filesystem_vfat::chmod(fsmount mnt, std::string_view path, mode_t mode) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_vfat>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non VFAT mount point");
            return -ENXIO;
        }
        const auto fspath = vmnt->native_path(path);
        const auto fret   = f_chmod(fspath.c_str(), translate_mode_to_attrib(mode), translate_mode_to_attrib(mode));
        return translate_error(fret);
    }

    auto filesystem_vfat::fchmod(fsfile zfile, mode_t mode) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_vfat>(zfile);
        if (!vfile) {
            LOG_ERROR("Non fat filesystem pointer");
            return -EBADF;
        }
        const auto fspath = vfile->open_path();
        const auto fret   = f_chmod(fspath.c_str(), translate_mode_to_attrib(mode), translate_mode_to_attrib(mode));
        return translate_error(fret);
    }

} // namespace purefs::fs::drivers
