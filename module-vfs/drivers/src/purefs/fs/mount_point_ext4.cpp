// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/drivers/mount_point_ext4.hpp>
#include <mutex.hpp>
#include <string>
#include <cstdlib>
#include <log/log.hpp>

namespace purefs::fs::drivers
{
    namespace
    {
        inline auto mount_path_mod(std::string_view path)
        {
            std::string ret{path};
            if (ret.back() != '/') {
                ret.push_back('/');
            }
            return ret;
        }
    } // namespace

    mount_point_ext4::mount_point_ext4(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                                       std::string_view path,
                                       unsigned flags,
                                       std::shared_ptr<filesystem_operations> fs)
        : mount_point(diskh, mount_path_mod(path), flags, fs), m_root(mount_path_mod(path)),
          m_lock(std::make_unique<cpp_freertos::MutexRecursive>())
    {}

    mount_point_ext4::~mount_point_ext4()
    {}

    auto mount_point_ext4::lock() noexcept -> void
    {
        m_lock->Lock();
    }

    auto mount_point_ext4::unlock() noexcept -> void
    {
        m_lock->Unlock();
    }

    ext4_locker::ext4_locker(std::shared_ptr<mount_point_ext4> mnt_ext) : m_mnt_ext(mnt_ext)
    {
        mnt_ext->lock();
    }

    ext4_locker::~ext4_locker()
    {
        auto ptr = m_mnt_ext.lock();
        if (ptr) {
            ptr->unlock();
        }
        else {
            LOG_FATAL("Unable to unlock ext4 filesystem");
            std::abort();
        }
    }

} // namespace purefs::fs::drivers
