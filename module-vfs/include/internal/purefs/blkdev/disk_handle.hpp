// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <string>
#include <purefs/blkdev/defs.hpp>

namespace purefs::blkdev
{
    class disk;
}

namespace purefs::blkdev::internal
{

    class disk_handle
    {
      public:
        static constexpr auto no_partition = -1;
        explicit disk_handle(std::weak_ptr<blkdev::disk> disk, std::string_view name, short partition = no_partition)
            : m_disk(disk), m_partition(partition), m_name(name)
        {}
        auto disk() const noexcept
        {
            return m_disk.lock();
        }
        auto partition() const noexcept
        {
            return m_partition;
        }
        auto has_partition() const noexcept -> bool
        {
            return m_partition != no_partition;
        }
        auto sectors() const noexcept -> sector_t;
        auto name() const noexcept
        {
            return m_name;
        }

      private:
        const std::weak_ptr<blkdev::disk> m_disk;
        const short m_partition{-1};
        mutable sector_t m_sectors{0};
        const std::string m_name;
    };
} // namespace purefs::blkdev::internal
