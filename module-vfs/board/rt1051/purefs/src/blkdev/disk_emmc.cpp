// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/blkdev/disk_emmc.hpp>
#include <unistd.h>
#include <cstring>
#include <task.h>

namespace purefs::blkdev
{
    disk_emmc::disk_emmc()
    {
        mmcCard.busWidth                   = kMMC_DataBusWidth8bit;
        mmcCard.busTiming                  = kMMC_HighSpeedTiming;
        mmcCard.enablePreDefinedBlockCount = true;
        mmcCard.host.base                  = USDHC2;
        mmcCard.host.sourceClock_Hz        = GetPerphSourceClock(PerphClock_USDHC2);
    }

    auto disk_emmc::probe(unsigned int flags) -> int
    {
        cpp_freertos::LockGuard lock(mutex);
        auto err = MMC_Init(&mmcCard);
        if (err != kStatus_Success) {
            initStatus = err;
            return initStatus;
        }
        return statusBlkDevSuccess;
    }

    auto disk_emmc::cleanup() -> int
    {
        cpp_freertos::LockGuard lock(mutex);
        if (!mmcCard.isHostReady) {
            return statusBlkDevFail;
        }
        MMC_Deinit(&mmcCard);
        return statusBlkDevSuccess;
    }

    auto disk_emmc::write(const void *buf, sector_t lba, std::size_t count) -> int
    {
        cpp_freertos::LockGuard lock(mutex);
        if (!mmcCard.isHostReady || buf == nullptr) {
            return statusBlkDevFail;
        }
        auto err = MMC_WriteBlocks(&mmcCard, static_cast<const uint8_t *>(buf), lba, count);
        if (err != kStatus_Success) {
            return err;
        }
        return statusBlkDevSuccess;
    }

    auto disk_emmc::erase(sector_t lba, std::size_t count) -> int
    {
        // erase group size is 512kB so it has been deliberately disallowed
        // group of this size would make the solution inefficient in this case
        return statusBlkDevSuccess;
    }
    auto disk_emmc::read(void *buf, sector_t lba, std::size_t count) -> int
    {
        cpp_freertos::LockGuard lock(mutex);
        if (!mmcCard.isHostReady || buf == nullptr) {
            return statusBlkDevFail;
        }
        auto err = MMC_ReadBlocks(&mmcCard, static_cast<uint8_t *>(buf), lba, count);
        if (err != kStatus_Success) {
            return err;
        }
        return statusBlkDevSuccess;
    }
    auto disk_emmc::sync() -> int
    {
        cpp_freertos::LockGuard lock(mutex);
        if (!mmcCard.isHostReady) {
            return statusBlkDevFail;
        }
        // Wait for the card's buffer to become empty
        while ((GET_SDMMCHOST_STATUS(mmcCard.host.base) & CARD_DATA0_STATUS_MASK) != CARD_DATA0_NOT_BUSY) {
            taskYIELD();
        }

        if (kStatus_Success != MMC_WaitWriteComplete(&mmcCard)) {
            return kStatus_SDMMC_WaitWriteCompleteFailed;
        }
        return statusBlkDevSuccess;
    }
    auto disk_emmc::status() const -> media_status
    {
        cpp_freertos::LockGuard lock(mutex);
        if (initStatus != kStatus_Success) {
            return media_status::error;
        }
        if (!mmcCard.isHostReady) {
            return media_status::uninit;
        }
        if ((mmcCard.csd.flags & kMMC_CsdPermanentWriteProtectFlag) ||
            (mmcCard.csd.flags & kMMC_CsdTemporaryWriteProtectFlag)) {
            return media_status::wprotect;
        }
        return media_status::healthly;
    }

    auto disk_emmc::get_info(info_type what) const -> scount_t
    {
        cpp_freertos::LockGuard lock(mutex);
        switch (what) {
        case info_type::sector_size:
            return mmcCard.blockSize;
        case info_type::sector_count:
            return mmcCard.userPartitionBlocks;
        case info_type::erase_block:
            // not supported
            return 0;
        }
        return -1;
    }
} // namespace purefs::blkdev
