// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/drivers/filesystem_vfat.hpp>
#include <purefs/fs/drivers/mount_point_vfat.hpp>
#include <log/log.hpp>
namespace purefs::fs::drivers
{

    namespace
    {
        /*
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
         */
    }
    auto filesystem_vfat::mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh, std::string_view path)
        -> fsmount
    {
        return std::make_shared<mount_point_vfat>(diskh, path, shared_from_this());
    }

    auto filesystem_vfat::mount(fsmount mnt) noexcept -> int
    {
        auto disk = mnt->disk().lock();
        if (!disk) {}
        return -1;
    }
} // namespace purefs::fs::drivers
