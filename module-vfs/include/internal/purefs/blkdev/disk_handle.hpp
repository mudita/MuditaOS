// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace purefs::blkdev::internal
{

    class disk_handle
    {
      public:
        static constexpr auto no_parition = -1;
        explicit disk_handle(std::size_t disk, short partition = no_parition) : m_disk(disk), m_partition(partition)
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

      private:
        const std::size_t m_disk{};
        const short m_partition{-1};
    };
} // namespace purefs::blkdev::internal
