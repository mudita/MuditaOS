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
                         unsigned flags,
                         std::shared_ptr<filesystem_operations> fs)
            : mount_point(diskh, path, flags, fs)
        {}
        virtual ~mount_point_vfat() = default;
        auto fatfs() const noexcept
        {
            return m_fatfs.get();
        }
        auto ff_drive(int lun) noexcept -> void
        {
            if (lun >= 0 && lun <= 9) {
                m_ff_drive[0] = lun + '0';
            }
            else {
                m_ff_drive[0] = ' ';
            }
        }
        auto ff_drive() const noexcept
        {
            return m_ff_drive;
        }
        auto ff_lun() const noexcept -> int
        {
            return (m_ff_drive[0] == ' ') ? (-1) : (m_ff_drive[0] - '0');
        }

      private:
        std::unique_ptr<::FATFS> m_fatfs;
        char m_ff_drive[3]{" :"};
    };
} // namespace purefs::fs::drivers
