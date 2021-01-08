// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/blkdev/disk.hpp>
#include <bsp/common.hpp>

#include "board/rt1051/bsp/eMMC/fsl_mmc.h"

#include <mutex.hpp>

#if !defined(TARGET_RT1051)
static_assert(false, "Unsupported target.");
#endif

namespace purefs::blkdev
{
    class disk_emmc final : public disk
    {
      public:
        static constexpr status_t statusBlkDevSuccess = 0;
        static constexpr status_t statusBlkDevFail    = -1;

        disk_emmc();

        auto probe(unsigned flags) -> int override;
        auto cleanup() -> int override;
        auto write(const void *buf, sector_t lba, std::size_t count) -> int override;
        auto read(void *buf, sector_t lba, std::size_t count) -> int override;
        auto sync() -> int override;
        auto status() const -> media_status override;
        auto get_info(info_type what) const -> scount_t override;
        auto erase(sector_t lba, std::size_t count) -> int override;

      private:
        status_t initStatus = kStatus_Success;

        mmc_card_t mmcCard;
        mutable cpp_freertos::MutexRecursive mutex;
    };
} // namespace purefs::blkdev
