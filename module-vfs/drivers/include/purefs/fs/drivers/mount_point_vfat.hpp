// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/fs/mount_point.hpp>
#include "ff.h"

namespace purefs::fs::drivers
{
    class mount_point_vfat final : public purefs::fs::internal::mount_point
    {
      public:
        mount_point_vfat(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                         std::string_view path,
                         std::shared_ptr<filesystem_operations> fs)
            : mount_point(diskh, path, fs)
        {}
        virtual ~mount_point_vfat() = default;
        auto fatfs() const noexcept
        {
            return m_fatfs.get();
        }

      private:
        std::unique_ptr<::FATFS> m_fatfs;
    };
} // namespace purefs::fs::drivers
