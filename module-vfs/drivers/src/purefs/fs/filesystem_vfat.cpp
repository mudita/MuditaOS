// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/drivers/filesystem_vfat.hpp>
#include <purefs/fs/drivers/mount_point_vfat.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_handle.hpp>
#include <log/log.hpp>
#include <volume_mapper.hpp>
#include <ff.h>

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
    } // namespace

    auto filesystem_vfat::mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                                         std::string_view path,
                                         unsigned flags) -> fsmount
    {
        return std::make_shared<mount_point_vfat>(diskh, path, flags, shared_from_this());
    }

    auto filesystem_vfat::mount(fsmount mnt) noexcept -> int
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
        filesystem_operations::mount(mnt);
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
        int ret = f_mount(nullptr, vmnt->ff_drive(), 1);
        ret     = translate_error(ret);
        if (!ret) {
            ret = ffat::internal::remove_volume(disk);
        }
        filesystem_operations::umount(mnt);
        return ret;
    }

    auto filesystem_vfat::stat_vfs(fsmount mnt, std::string_view path, statvfs &stat) const noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::open(fsmount mnt, std::string_view path, int flags, int mode) noexcept -> fsfile
    {
        return nullptr;
    }

    auto filesystem_vfat::close(fsfile zfile) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::write(fsfile zfile, const char *ptr, size_t len) noexcept -> ssize_t
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::read(fsfile zfile, char *ptr, size_t len) noexcept -> ssize_t
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::fstat(fsfile zfile, struct stat &st) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::stat(fsmount mnt, std::string_view file, struct stat &st) noexcept -> int
    {
        return -ENOTSUP;
    }
    auto filesystem_vfat::link(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::symlink(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::unlink(fsmount mnt, std::string_view name) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::rename(fsmount mnt, std::string_view oldname, std::string_view newname) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::mkdir(fsmount mnt, std::string_view path, int mode) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::diropen(fsmount mnt, std::string_view path) noexcept -> fsdir
    {
        return nullptr;
    }

    auto filesystem_vfat::dirreset(fsdir dirstate) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::dirnext(fsdir dirstate, std::string &filename, struct stat &filestat) -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::dirclose(fsdir dirstate) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::ftruncate(fsfile zfile, off_t len) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::fsync(fsfile zfile) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::ioctl(fsmount mnt, std::string_view path, int cmd, void *arg) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::utimens(fsmount mnt, std::string_view path, std::array<timespec, 2> &tv) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::flock(fsfile zfile, int cmd) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::isatty(fsfile zfile) noexcept -> int
    {
        return 0;
    }

    auto filesystem_vfat::chmod(fsmount mnt, std::string_view path, mode_t mode) noexcept -> int
    {
        return -ENOTSUP;
    }

    auto filesystem_vfat::fchmod(fsfile zfile, mode_t mode) noexcept -> int
    {
        return -ENOTSUP;
    }

} // namespace purefs::fs::drivers
