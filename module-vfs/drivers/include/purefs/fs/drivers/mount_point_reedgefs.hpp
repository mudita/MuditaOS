// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/fs/mount_point.hpp>

namespace purefs::fs::drivers
{
    class mount_point_reedgefs final : public purefs::fs::internal::mount_point
    {
      public:
        mount_point_reedgefs(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                             std::string_view path,
                             unsigned flags,
                             std::shared_ptr<filesystem_operations> fs)
            : mount_point(diskh, path, flags, fs)
        {}
        virtual ~mount_point_reedgefs() = default;

        auto volume_name(std::string volume_name) -> void
        {
            m_volume_name = volume_name;
        }

        auto volume_name() const noexcept -> std::string
        {
            return m_volume_name;
        }

      private:
        auto native_root() const noexcept -> std::string override
        {
            return volume_name();
        }
        std::string m_volume_name;
    };
} // namespace purefs::fs::drivers
