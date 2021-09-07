// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/blkdev/disk.hpp>
#include <mutex.hpp>

#if !defined(TARGET_RT1051)
#error Unsupported target
#endif

namespace purefs::blkdev
{
    class disk_eeprom final : public disk
    {
      public:
        explicit disk_eeprom(int _bus_id) : bus_id(_bus_id)
        {}
        virtual ~disk_eeprom() = default;
        auto probe(unsigned flags) -> int override;
        auto write(const void *buf, sector_t lba, std::size_t count, hwpart_t hwpart) -> int override;
        auto read(void *buf, sector_t lba, std::size_t count, hwpart_t hwpart) -> int override;
        auto status() const -> media_status override;
        auto get_info(info_type what, hwpart_t hwpart) const -> scount_t override;

      private:
        mutable cpp_freertos::MutexRecursive mutex;
        const int bus_id;
    };
} // namespace purefs::blkdev
