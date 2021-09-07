// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once
#include <purefs/fs/mount_point.hpp>
#include <lfs.h>

namespace purefs::fs::drivers
{

    class mount_point_littlefs final : public purefs::fs::internal::mount_point
    {
      public:
        mount_point_littlefs(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                             std::string_view path,
                             unsigned flags,
                             std::shared_ptr<filesystem_operations> fs)
            : mount_point(diskh, path, flags, fs)
        {}
        virtual ~mount_point_littlefs() = default;

        [[nodiscard]] auto lfs_config() noexcept
        {
            return &m_lfs_conf;
        }
        [[nodiscard]] auto lfs_mount() noexcept
        {
            return &m_lfs_mount;
        }

      private:
        auto native_root() const noexcept -> std::string_view override
        {
            return "";
        }
        struct lfs_config m_lfs_conf
        {};
        lfs_t m_lfs_mount{};
    };
} // namespace purefs::fs::drivers
