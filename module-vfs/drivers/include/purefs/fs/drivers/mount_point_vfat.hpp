// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        [[nodiscard]] auto fatfs() noexcept
        {
            return &m_fatfs;
        }
        auto ff_drive(int lun) noexcept -> void
        {
            static constexpr auto drive_letter_min = 0;
            static constexpr auto drive_letter_max = 9;
            if (lun >= drive_letter_min && lun <= drive_letter_max) {
                m_ff_drive[0] = lun + '0';
            }
            else {
                m_ff_drive[0] = ' ';
            }
        }
        [[nodiscard]] auto ff_drive() const noexcept
        {
            return m_ff_drive;
        }
        [[nodiscard]] auto ff_lun() const noexcept -> int
        {
            return (m_ff_drive[0] == ' ') ? (-1) : (m_ff_drive[0] - '0');
        }

      private:
        auto native_root() const noexcept -> std::string override
        {
            return ff_drive();
        }

      private:
        static constexpr auto disk_name_size = 3;
        ::FATFS m_fatfs{};
        char m_ff_drive[disk_name_size]{" :"};
    };
} // namespace purefs::fs::drivers
