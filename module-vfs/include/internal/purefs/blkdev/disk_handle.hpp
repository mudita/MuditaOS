// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <string>
#include <limits>
#include <purefs/blkdev/defs.hpp>

namespace purefs::blkdev
{
    class disk;
}

namespace purefs::blkdev::internal
{

    class disk_handle
    {
        static constexpr part_t no_partition     = -1;
        static constexpr auto hw_partition_first = std::numeric_limits<part_t>::max() / 2;

      public:
        static constexpr auto to_syspart_num(part_t part) -> part_t
        {
            return part + hw_partition_first;
        }
        static auto is_any_partition(part_t part) noexcept -> bool
        {
            return part >= 0;
        }
        static auto is_system_partition(part_t part) noexcept -> bool
        {
            return part >= hw_partition_first;
        }
        static auto is_user_partition(part_t part) noexcept -> bool
        {
            return is_any_partition(part) && !is_system_partition(part);
        }
        explicit disk_handle(std::weak_ptr<blkdev::disk> disk, std::string_view name, part_t partition = no_partition)
            : m_disk(disk), m_partition(partition), m_name(name)
        {}
        auto is_any_partition() const noexcept -> bool
        {
            return is_any_partition(m_partition);
        }
        auto is_system_partition() const noexcept -> bool
        {
            return is_system_partition(m_partition);
        }
        auto is_user_partition() const noexcept -> bool
        {
            return is_user_partition(m_partition);
        }
        auto disk() const noexcept
        {
            return m_disk.lock();
        }
        auto partition() const noexcept
        {
            return m_partition;
        }
        auto system_partition() const noexcept -> int
        {
            return (m_partition >= hw_partition_first) ? (m_partition - hw_partition_first + 1) : (0);
        }
        auto sectors() const noexcept -> sector_t;
        auto name() const noexcept
        {
            return m_name;
        }

      private:
        const std::weak_ptr<blkdev::disk> m_disk;
        const part_t m_partition{no_partition};
        mutable sector_t m_sectors{0};
        const std::string m_name;
    };
} // namespace purefs::blkdev::internal
