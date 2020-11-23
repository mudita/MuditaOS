// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

namespace purefs::blkdev
{
    class disk;
}

namespace purefs::blkdev::internal
{

    class disk_handle
    {
      public:
        static constexpr auto no_parition = -1;
        explicit disk_handle(std::weak_ptr<blkdev::disk> disk, sector_t sectors, short partition = no_parition)
            : m_disk(disk), m_partition(partition), m_sectors(sectors)
        {}
        auto disk() const noexcept
        {
            return m_disk;
        }
        auto partition() const noexcept
        {
            return m_partition;
        }
        auto has_partition() const noexcept
        {
            return m_partition != no_parition;
        }
        auto sectors() const noexcept
        {
            return m_sectors;
        }

      private:
        std::weak_ptr<blkdev::disk> m_disk;
        const short m_partition{-1};
        const sector_t m_sectors;
    };
} // namespace purefs::blkdev::internal
