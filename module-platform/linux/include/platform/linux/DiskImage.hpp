// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/blkdev/disk.hpp>
#include <mutex>
#include <vector>

namespace purefs::blkdev
{
    class disk_image final : public disk
    {
        static constexpr auto invalid_fd   = -1;
        static constexpr auto syspart_size = 32 * 1024UL * 1024UL;

      public:
        explicit disk_image(std::string_view image_filename, std::size_t sector_size = 512, hwpart_t num_parts = 8);
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
        auto pm_control(pm_state target_state) -> int override;
        auto pm_read(pm_state &current_state) -> int override;
        auto range_valid(sector_t lba, std::size_t count, hwpart_t hwpart) const -> bool;
        auto open_and_truncate(hwpart_t hwpart) -> int;

      private:
        pm_state pmState{pm_state::active};
        std::vector<int> m_filedes;
        std::vector<std::size_t> m_sectors;
        const std::string m_image_name;
        const std::size_t m_sector_size;
        const std::size_t m_syspart_sectors;
        const hwpart_t m_sysparts;
        mutable std::recursive_mutex m_mtx;
    };
} // namespace purefs::blkdev
