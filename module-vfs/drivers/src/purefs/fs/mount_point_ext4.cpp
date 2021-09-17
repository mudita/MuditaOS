// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/drivers/mount_point_ext4.hpp>
#include <mutex.hpp>
#include <string>

namespace purefs::fs::drivers
{
    mount_point_ext4::mount_point_ext4(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                                       std::string_view path,
                                       unsigned flags,
                                       std::shared_ptr<filesystem_operations> fs)
        : mount_point(diskh, path, flags, fs), m_root(path), m_lock(std::make_unique<cpp_freertos::MutexRecursive>())
    {}

    mount_point_ext4::~mount_point_ext4()
    {}
    void mount_point_ext4::lock() noexcept
    {
        m_lock->Lock();
    }
    void mount_point_ext4::unlock() noexcept
    {
        m_lock->Unlock();
    }

} // namespace purefs::fs::drivers