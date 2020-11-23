// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/blkdev/disk.hpp>

namespace purefs::blkdev
{
    class disk_image final : public disk
    {
        static constexpr auto file_name   = "PurePhone.img";
        static constexpr auto sector_size = 512UL;

      public:
        virtual ~disk_image()
        {}

      private:
        auto probe(unsigned flags) -> int override;
        auto cleanup() -> int override;
        auto write(const void *buf, sector_t lba, std::size_t count) -> int override;
        auto read(void *buf, sector_t lba, std::size_t count) -> int override;
        auto sync() -> int override;
        auto status() const -> media_status override;
        auto get_info(info_type what) const -> scount_t override;

      private:
        int m_filedes{-1};
        unsigned long m_sectors{0};
    };
} // namespace purefs::blkdev