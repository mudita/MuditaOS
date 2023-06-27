// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/usdhc/DriverUSDHC.hpp"
#include <purefs/blkdev/disk.hpp>
#include <mutex.hpp>
#include <memory>
#include <atomic>
#include <map>

#if !defined(TARGET_RT1051)
static_assert(false, "Unsupported target.");
#endif

struct _mmc_card;

namespace purefs::blkdev
{
    class disk_emmc final : public disk
    {
      public:
        struct disk_emmc_info
        {
            std::string manufacturer;
            std::uint32_t blocksCount;
            std::uint32_t blockSize;
            std::uint64_t capacity;
            std::string name;
            std::uint8_t versionMajor;
            std::uint8_t versionMinor;
            std::uint32_t serialNumber;
            std::uint8_t productionMonth;
            std::uint16_t productionYear;
        };

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
        auto pm_control(pm_state target_state) -> int override;
        auto pm_read(pm_state &current_state) -> int override;
        auto get_emmc_info() const -> disk_emmc_info;
        auto get_emmc_info_str() const -> std::string;

      private:
        auto switch_partition(hwpart_t newpart) -> int;
        auto get_emmc_manufacturer() const -> std::string;

        int initStatus;
        pm_state pmState{pm_state::active};
        mutable cpp_freertos::MutexRecursive mutex;
        std::atomic<hwpart_t> currHwPart{0};

        std::unique_ptr<_mmc_card> mmcCard;
        std::shared_ptr<drivers::DriverUSDHC> driverUSDHC;

        /* Read from datasheets */
        const std::map<std::uint8_t, std::string> mmcManufacturersMap{
            {0x15, "Samsung"}, {0x70, "Kingston"}, {0xF4, "Biwin"}};
    };
} // namespace purefs::blkdev
