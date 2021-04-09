// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/blkdev/disk.hpp>
#include <mutex>
#include <array>

namespace purefs::blkdev
{
    class disk_image final : public disk
    {
        static constexpr auto sector_size = 512UL;
        static constexpr auto invalid_fd      = -1;
        static constexpr auto syspart_count   = 8U;
        static constexpr auto syspart_size    = 256UL * 1024UL * 1024UL;
        static constexpr auto syspart_sectors = syspart_size / sector_size;

      public:
        disk_image(std::string_view image_filename);
        virtual ~disk_image()
        {}

      private:
        auto probe(unsigned flags) -> int override;
        auto cleanup() -> int override;
        auto write(const void *buf, sector_t lba, std::size_t count, hwpart_t hwpart) -> int override;
        auto read(void *buf, sector_t lba, std::size_t count, hwpart_t hwpart) -> int override;
        auto sync() -> int override;
        auto status() const -> media_status override;
        auto get_info(info_type what, hwpart_t hwpart) const -> scount_t override;
        auto erase(sector_t lba, std::size_t count, hwpart_t hwpart) -> int override;
        auto range_valid(sector_t lba, std::size_t count, hwpart_t hwpart) const -> bool;
        auto open_and_truncate(hwpart_t hwpart) -> int;

      private:
        std::array<int, syspart_count> m_filedes;
        std::array<unsigned long, syspart_count> m_sectors;
        const std::string m_image_name;
        mutable std::recursive_mutex m_mtx;
    };
} // namespace purefs::blkdev
