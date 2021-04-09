// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/blkdev/disk.hpp>
#include <mutex.hpp>
#include <memory>
#include <atomic>

#if !defined(TARGET_RT1051)
static_assert(false, "Unsupported target.");
#endif

struct _mmc_card;

namespace purefs::blkdev
{
    class disk_emmc final : public disk
    {
      public:
        static constexpr auto statusBlkDevSuccess = 0;
        static constexpr auto statusBlkDevFail    = -1;

        disk_emmc();
        virtual ~disk_emmc();

        auto probe(unsigned flags) -> int override;
        auto cleanup() -> int override;
        auto write(const void *buf, sector_t lba, std::size_t count, hwpart_t hwpart) -> int override;
        auto read(void *buf, sector_t lba, std::size_t count, hwpart_t hwpart) -> int override;
        auto sync() -> int override;
        auto status() const -> media_status override;
        auto get_info(info_type what, hwpart_t hwpart) const -> scount_t override;
        auto erase(sector_t lba, std::size_t count, hwpart_t hwpart) -> int override;

      private:
        auto switch_partition(hwpart_t newpart) -> int;

      private:
        int initStatus;
        std::unique_ptr<_mmc_card> mmcCard;
        mutable cpp_freertos::MutexRecursive mutex;
        std::atomic<hwpart_t> currHwPart{0};
    };
} // namespace purefs::blkdev
