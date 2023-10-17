// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/drivers/filesystem_reedgefs.hpp>
#include <purefs/fs/drivers/mount_point_reedgefs.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/fs/drivers/file_handle_reedgefs.hpp>
#include <purefs/fs/drivers/directory_handle_reedgefs.hpp>
#include <purefs/fs/mount_flags.hpp>
#include <log/log.hpp>
#include <reedgefs/volume_mapper.hpp>
#include <sys/statvfs.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <redfs.h>
#include <redposix.h>

#include <cstring>

namespace purefs::fs::drivers
{
    namespace
    {
        int translate_error(int error)
        {
            switch (error) {
            case -RED_ENOTSUPP:
                return -ENOTSUP;
            default:
                return error;
            }
        }

        uint8_t translate_flags(unsigned flags)
        {
            uint8_t mode = 0;
            switch (flags & O_ACCMODE) {
            case O_RDONLY:
                mode |= RED_O_RDONLY;
                break;
            case O_WRONLY:
                mode |= RED_O_WRONLY;
                break;
            case O_RDWR:
                mode |= RED_O_RDWR;
                break;
            }

            if (flags & O_APPEND) {
                mode |= RED_O_APPEND;
            }
            if (flags & O_CREAT) {
                mode |= RED_O_CREAT;
            }
            if (flags & O_TRUNC) {
                mode |= RED_O_TRUNC;
            }
            if (flags & O_EXCL) {
                mode |= RED_O_EXCL;
            }
            return mode;
        }

        auto translate_mode_to_st_mode(uint16_t _mode, bool readOnly)
        {
            decltype(std::declval<struct stat *>()->st_mode) mode = _mode | S_IRUSR | S_IRGRP | S_IROTH;
            if (!readOnly) {
                mode |= S_IWUSR | S_IWGRP | S_IWOTH;
            }
            return mode;
        }

        void translate_redstat_to_stat(const REDSTAT &fs, uint32_t blksize, struct stat &st, bool ro)
        {
            std::memset(&st, 0, sizeof st);
            st.st_dev   = fs.st_dev;
            st.st_ino   = fs.st_ino;
            st.st_mode  = translate_mode_to_st_mode(fs.st_mode, ro);
            st.st_nlink = fs.st_nlink;
            st.st_uid   = 0;
            st.st_gid   = 0;
            st.st_rdev  = 0;
            st.st_size  = fs.st_size;

            st.st_blksize = blksize;
            st.st_blocks  = fs.st_blocks;
            // For some reason compiler returns error here even if REDCONF_INODE_TIMESTAMPS is set to 1
            // #if REDCONF_INODE_TIMESTAMPS == 1
            //             st.st_atime = fs.st_atime;
            //             st.st_mtime = fs.st_mtime;
            //             st.st_ctime = fs.st_ctime;
            // #endif
        }
    } // namespace

    filesystem_reedgefs::filesystem_reedgefs()
    {
        red_init();
    }

    filesystem_reedgefs::~filesystem_reedgefs()
    {
        red_uninit();
    }

    auto filesystem_reedgefs::mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                                             std::string_view path,
                                             unsigned flags) -> fsmount
    {
        return std::make_shared<mount_point_reedgefs>(diskh, path, flags, shared_from_this());
    }

    auto filesystem_reedgefs::mount(fsmount mnt, const void *data) noexcept -> int
    {
        auto disk = mnt->disk();
        if (!disk) {
            return -EIO;
        }
        auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non reedgefs mount point");
            return -EIO;
        }
        const auto mountPath   = vmnt->mount_path();
        auto [volumeName, ret] = reedgefs::internal::append_volume(disk, mountPath);
        if (ret < 0) {
            LOG_ERROR("Unable to attach volume to ff layer with errno %i", ret);
            return ret;
        }
        // TODO: This is temporary fix, need to be removed when image creation is ready
        red_format(volumeName.c_str());
        // TODO: ////////////////////////////////

        if (red_mount(volumeName.c_str()) < 0) {
            return translate_error(-red_errno);
        }
        vmnt->volume_name(volumeName);
        filesystem_operations::mount(mnt, data);
        return ret;
    }

    auto filesystem_reedgefs::filesystem_register_completed() const noexcept -> int
    {
        const auto dmgr = disk_mngr();
        if (dmgr) {
            reedgefs::internal::reset_volumes(dmgr);
            return 0;
        }
        else {
            return -EIO;
        }
    }

    auto filesystem_reedgefs::umount(fsmount mnt) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non reedgefs mount point");
            return -EIO;
        }

        auto disk = mnt->disk();
        if (!disk) {
            return -EIO;
        }

        int ret = red_umount(vmnt->volume_name().c_str());
        if (ret < 0) {
            ret = translate_error(-red_errno);
        }
        if (!ret) {
            ret = reedgefs::internal::remove_volume(disk);
            filesystem_operations::umount(mnt);
        }
        return ret;
    }

    auto filesystem_reedgefs::stat_vfs(fsmount mnt, std::string_view, struct statvfs &stat) const noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non reedgefs mount point");
            return -EIO;
        }
        REDSTATFS statvfs;
        auto ret = red_statvfs(vmnt->volume_name().c_str(), &statvfs);
        if (ret != 0) {
            return translate_error(-red_errno);
        }

        stat.f_bsize   = statvfs.f_bsize;
        stat.f_frsize  = statvfs.f_frsize;
        stat.f_blocks  = statvfs.f_blocks;
        stat.f_bfree   = statvfs.f_bfree;
        stat.f_bavail  = statvfs.f_bavail;
        stat.f_flag    = statvfs.f_flag;
        stat.f_files   = statvfs.f_files;
        stat.f_ffree   = statvfs.f_ffree;
        stat.f_favail  = statvfs.f_favail;
        stat.f_fsid    = statvfs.f_fsid;
        stat.f_namemax = statvfs.f_namemax;
        return 0;
    }

    auto filesystem_reedgefs::open(fsmount mnt, std::string_view path, int flags, int mode) noexcept -> fsfile
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non reedgefs mount point");
            return nullptr;
        }
        const auto fspath  = vmnt->native_path(path);
        const auto fsflags = translate_flags(flags);
        auto fileo         = std::make_shared<file_handle_reedgefs>(mnt, fspath, flags);
        auto ret           = red_open(fspath.c_str(), fsflags);
        if (ret < 0) {
            fileo->error(translate_error(-red_errno));
        }
        else {
            fileo->fd(ret);
        }

        return fileo;
    }

    auto filesystem_reedgefs::close(fsfile zfile) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_reedgefs>(zfile);
        if (!vfile) {
            LOG_ERROR("Non reedgefs filesystem pointer");
            return -EBADF;
        }
        const auto ret = red_close(vfile->fd());
        if (ret < 0) {
            return translate_error(-red_errno);
        }
        return 0;
    }

    auto filesystem_reedgefs::write(fsfile zfile, const char *ptr, size_t len) noexcept -> ssize_t
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_reedgefs>(zfile);
        if (!vfile) {
            LOG_ERROR("Non reedgefs filesystem pointer");
            return -EBADF;
        }

        const auto ret = red_write(vfile->fd(), ptr, len);
        if (ret < 0) {
            return translate_error(-red_errno);
        }

        return ret;
    }

    auto filesystem_reedgefs::read(fsfile zfile, char *ptr, size_t len) noexcept -> ssize_t
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_reedgefs>(zfile);
        if (!vfile) {
            LOG_ERROR("Non reedgefs filesystem pointer");
            return -EBADF;
        }
        const auto ret = red_read(vfile->fd(), ptr, len);
        if (ret < 0) {
            return translate_error(-red_errno);
        }

        return ret;
    }

    auto filesystem_reedgefs::seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_reedgefs>(zfile);
        if (!vfile) {
            LOG_ERROR("Non reedgefs filesystem pointer");
            return -EBADF;
        }

        auto ret = red_lseek(vfile->fd(), pos, static_cast<REDWHENCE>(dir));
        if (ret < 0) {
            return translate_error(-red_errno);
        }

        return ret;
    }

    auto filesystem_reedgefs::fstat(fsfile zfile, struct stat &st) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_reedgefs>(zfile);
        if (!vfile) {
            LOG_ERROR("Non reedgefs filesystem pointer");
            return -EBADF;
        }
        REDSTAT redstat;
        const auto ret = red_fstat(vfile->fd(), &redstat);
        if (ret < 0) {
            return translate_error(-red_errno);
        }

        const auto mnt = vfile->mntpoint();
        if (mnt) {
            auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
            if (!vmnt) {
                LOG_ERROR("Non reedgefs mount point");
                return -EIO;
            }
            REDSTATFS redstatfs;
            auto ret = red_statvfs(vmnt->volume_name().c_str(), &redstatfs);
            if (ret != 0) {
                return translate_error(-red_errno);
            }
            translate_redstat_to_stat(redstat, redstatfs.f_bsize, st, vmnt->is_ro());
        }
        else {
            return -EIO;
        }

        return 0;
    }

    auto filesystem_reedgefs::stat(fsmount mnt, std::string_view file, struct stat &st) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non reedgefs mount point");
            return -EBADF;
        }

        const auto fspath = vmnt->native_path(file);
        auto fh           = red_open(fspath.c_str(), RED_O_RDONLY);
        if (fh < 0) {
            return translate_error(-red_errno);
        }
        REDSTAT redstat;
        auto ret = red_fstat(fh, &redstat);
        if (ret < 0) {
            return translate_error(-red_errno);
        }
        REDSTATFS redstatfs;
        ret = red_statvfs(vmnt->volume_name().c_str(), &redstatfs);
        if (ret != 0) {
            return translate_error(-red_errno);
        }
        translate_redstat_to_stat(redstat, redstatfs.f_bsize, st, vmnt->is_ro());
        ret = red_close(fh);
        if (ret < 0) {
            return translate_error(-red_errno);
        }
        return 0;
    }

    auto filesystem_reedgefs::unlink(fsmount mnt, std::string_view name) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non reedgefs mount point");
            return -ENXIO;
        }
        const auto fspath = vmnt->native_path(name);
        const auto fret   = red_unlink(fspath.c_str());
        if (fret != 0) {
            return translate_error(-red_errno);
        }
        return fret;
    }

    auto filesystem_reedgefs::rename(fsmount mnt, std::string_view oldname, std::string_view newname) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non reedgefs mount point");
            return -ENXIO;
        }
        const auto fsold = vmnt->native_path(oldname);
        const auto fsnew = vmnt->native_path(newname);
        const auto fret  = red_rename(fsold.c_str(), fsnew.c_str());
        if (fret != 0) {
            return translate_error(-red_errno);
        }
        return fret;
    }

    auto filesystem_reedgefs::mkdir(fsmount mnt, std::string_view path, int mode) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non reedgefs mount point");
            return -ENXIO;
        }
        const auto fspath = vmnt->native_path(path);
        const auto fret   = red_mkdir(fspath.c_str());
        if (fret != 0) {
            return translate_error(-red_errno);
        }
        return fret;
    }

    auto filesystem_reedgefs::diropen(fsmount mnt, std::string_view path) noexcept -> fsdir
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non reedgefs mount point");
            return nullptr;
        }
        const auto fspath = vmnt->native_path(path);
        const auto dirp   = std::make_shared<directory_handle_reedgefs>(mnt, 0);
        const auto fret   = red_opendir(fspath.c_str());
        if (fret == nullptr) {
            dirp->error(translate_error(-red_errno));
        }
        else {
            dirp->reedgefs_dirp(fret);
        }

        return dirp;
    }

    auto filesystem_reedgefs::dirreset(fsdir dirstate) noexcept -> int
    {
        auto dirp = std::dynamic_pointer_cast<directory_handle_reedgefs>(dirstate);
        if (!dirp) {
            LOG_ERROR("Non reedgefs directory handle");
            return -ENXIO;
        }
        red_rewinddir(dirp->reedgefs_dirp());
        return 0;
    }

    auto filesystem_reedgefs::dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) -> int
    {
        auto dirp = std::dynamic_pointer_cast<directory_handle_reedgefs>(dirstate);
        if (!dirp) {
            LOG_ERROR("Non reedgefs directory handle");
            return -ENXIO;
        }
        red_errno      = 0;
        auto reddirent = red_readdir(dirp->reedgefs_dirp());
        if (reddirent != nullptr) {
            if (reddirent->d_name[0] == '\0') {
                return -ENODATA;
            }
            else {
                const auto mnt = dirp->mntpoint();
                if (mnt) {
                    auto vmnt = std::dynamic_pointer_cast<mount_point_reedgefs>(mnt);
                    if (!vmnt) {
                        LOG_ERROR("Non reedgefs mount point");
                        return -EIO;
                    }

                    REDSTATFS redstatfs;
                    auto ret = red_statvfs(vmnt->volume_name().c_str(), &redstatfs);
                    if (ret != 0) {
                        return translate_error(-red_errno);
                    }
                    translate_redstat_to_stat(reddirent->d_stat, redstatfs.f_bsize, filestat, vmnt->is_ro());
                    filename = reddirent->d_name;
                    return 0;
                }
                else {
                    return -EIO;
                }
            }
        }
        return red_errno != 0 ? translate_error(-red_errno) : -ENODATA;
    }

    auto filesystem_reedgefs::dirclose(fsdir dirstate) noexcept -> int
    {
        auto dirp = std::dynamic_pointer_cast<directory_handle_reedgefs>(dirstate);
        if (!dirp) {
            LOG_ERROR("Not a reedgefs directory handle");
            return -ENXIO;
        }
        const auto fret = red_closedir(dirp->reedgefs_dirp());
        if (fret != 0) {
            return translate_error(-red_errno);
        }
        return fret;
    }

    auto filesystem_reedgefs::ftruncate(fsfile zfile, off_t len) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_reedgefs>(zfile);
        if (!vfile) {
            LOG_ERROR("Non reedgefs filesystem pointer");
            return -EBADF;
        }
        const int fret = red_ftruncate(vfile->fd(), len);
        if (fret != 0) {
            return translate_error(-red_errno);
        }
        return fret;
    }

    auto filesystem_reedgefs::fsync(fsfile zfile) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_reedgefs>(zfile);
        if (!vfile) {
            LOG_ERROR("Non reedgefs filesystem pointer");
            return -EBADF;
        }
        const int fret = red_fsync(vfile->fd());
        if (fret != 0) {
            return translate_error(-red_errno);
        }
        return fret;
    }

    auto filesystem_reedgefs::isatty(fsfile zfile) noexcept -> int
    {
        // NOTE: Handle is always not a tty
        return 0;
    }

} // namespace purefs::fs::drivers
