// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/blkdev/defs.hpp>

#include <vector>
#include <memory>
#include <array>

namespace purefs::blkdev
{
    class disk;
    class partition;

    namespace internal
    {
        class partition_parser
        {
          public:
            partition_parser(std::shared_ptr<disk> disk, std::vector<partition> &parts)
                : m_disk(std::move(disk)), m_parts(parts)
            {}
            partition_parser(const partition_parser &) = delete;
            auto operator=(const partition_parser &) -> partition_parser & = delete;

            auto partition_search() -> int;

          private:
            static auto read_partitions(const std::vector<std::uint8_t> &buffer, std::array<partition, 4> &parts)
                -> void;
            static auto is_extended(std::uint8_t type) -> bool;
            auto parse_extended(std::uint32_t lba, std::uint32_t count) -> int;
            auto check_partition(const std::shared_ptr<disk> disk, const partition &part) -> bool;

          private:
            const std::shared_ptr<disk> m_disk;
            std::vector<partition> &m_parts;
        };
    } // namespace internal
} // namespace purefs::blkdev
