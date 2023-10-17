// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <memory>
#include <purefs/fs/mount_point.hpp>

struct ext4_blockdev;
namespace cpp_freertos
{
    class MutexRecursive;
}

namespace purefs::fs::drivers
{
    class mount_point_ext4 final : public purefs::fs::internal::mount_point
    {
      public:
        mount_point_ext4(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                         std::string_view path,
                         unsigned flags,
                         std::shared_ptr<filesystem_operations> fs);
        virtual ~mount_point_ext4();
        [[nodiscard]] auto block_dev() const noexcept
        {
            return m_bdev;
        }
        auto block_dev(ext4_blockdev *bdev) noexcept -> void
        {
            m_bdev = bdev;
        }
        auto lock() noexcept -> void;
        auto unlock() noexcept -> void;

      private:
        auto native_root() const noexcept -> std::string override
        {
            return m_root;
        }
        ext4_blockdev *m_bdev{};
        const std::string m_root;
        std::unique_ptr<cpp_freertos::MutexRecursive> m_lock;
    };

    // Fs locker
    class ext4_locker
    {
      public:
        explicit ext4_locker(std::shared_ptr<mount_point_ext4> mnt_ext);
        ~ext4_locker();
        ext4_locker(ext4_locker &)  = delete;
        ext4_locker(ext4_locker &&) = delete;
        ext4_locker &operator=(ext4_locker &) = delete;
        ext4_locker &operator=(ext4_locker &&) = delete;

      private:
        const std::weak_ptr<mount_point_ext4> m_mnt_ext;
    };
} // namespace purefs::fs::drivers
