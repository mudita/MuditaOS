// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "disk_emmc.hpp"

#include "board/rt1051/bsp/eMMC/fsl_mmc.h"
#include "board/BoardDefinitions.hpp"
#include <log/log.hpp>
#include <Utils.hpp>

#include <cstring>
#include <task.h>

namespace purefs::blkdev
{
    disk_emmc::disk_emmc() : initStatus(kStatus_Success), mmcCard(std::make_unique<_mmc_card>())
    {
        assert(mmcCard.get());
        std::memset(mmcCard.get(), 0, sizeof(_mmc_card));
        mmcCard->busWidth                   = kMMC_DataBusWidth8bit;
        mmcCard->busTiming                  = kMMC_HighSpeedTiming;
        mmcCard->enablePreDefinedBlockCount = true;
        mmcCard->host.base                  = USDHC2;
        mmcCard->host.sourceClock_Hz        = GetPerphSourceClock(PerphClock_USDHC2);

        driverUSDHC = drivers::DriverUSDHC::Create(
            "EMMC", static_cast<drivers::USDHCInstances>(BoardDefinitions::EMMC_USDHC_INSTANCE));
    }

    disk_emmc::~disk_emmc()
    {}

    auto disk_emmc::probe([[maybe_unused]] unsigned int flags) -> int
    {
        cpp_freertos::LockGuard lock(mutex);
        const auto err = MMC_Init(mmcCard.get());
        if (err != kStatus_Success) {
            initStatus = err;
            return initStatus;
        }

        LOG_INFO("\neMMC card info:\n%s", get_emmc_info_str().c_str());

        return statusBlkDevSuccess;
    }

    auto disk_emmc::cleanup() -> int
    {
        cpp_freertos::LockGuard lock(mutex);
        if (!mmcCard->isHostReady) {
            return statusBlkDevFail;
        }
        MMC_Deinit(mmcCard.get());
        return statusBlkDevSuccess;
    }

    auto disk_emmc::write(const void *buf, sector_t lba, std::size_t count, hwpart_t hwpart) -> int
    {
        cpp_freertos::LockGuard lock(mutex);
        if (!mmcCard->isHostReady || buf == nullptr) {
            return statusBlkDevFail;
        }
        auto err = switch_partition(hwpart);
        if (err != kStatus_Success) {
            return err;
        }
        if (pmState == pm_state::suspend) {
            driverUSDHC->Enable();
        }
        err = MMC_WriteBlocks(mmcCard.get(), static_cast<const uint8_t *>(buf), lba, count);
        if (pmState == pm_state::suspend) {
            driverUSDHC->Disable();
        }
        if (err != kStatus_Success) {
            return err;
        }
        return statusBlkDevSuccess;
    }

    auto disk_emmc::read(void *buf, sector_t lba, std::size_t count, hwpart_t hwpart) -> int
    {
        cpp_freertos::LockGuard lock(mutex);
        if (!mmcCard->isHostReady || buf == nullptr) {
            return statusBlkDevFail;
        }
        auto err = switch_partition(hwpart);
        if (err != kStatus_Success) {
            return err;
        }
        if (pmState == pm_state::suspend) {
            driverUSDHC->Enable();
        }
        err = MMC_ReadBlocks(mmcCard.get(), static_cast<uint8_t *>(buf), lba, count);
        if (pmState == pm_state::suspend) {
            driverUSDHC->Disable();
        }
        if (err != kStatus_Success) {
            return err;
        }
        return statusBlkDevSuccess;
    }
    auto disk_emmc::sync() -> int
    {
        cpp_freertos::LockGuard lock(mutex);
        if (!mmcCard->isHostReady) {
            return statusBlkDevFail;
        }
        // Wait for the card's buffer to become empty
        while ((GET_SDMMCHOST_STATUS(mmcCard->host.base) & CARD_DATA0_STATUS_MASK) != CARD_DATA0_NOT_BUSY) {
            taskYIELD();
        }
        if (pmState == pm_state::suspend) {
            driverUSDHC->Enable();
        }
        auto err = MMC_WaitWriteComplete(mmcCard.get());
        if (pmState == pm_state::suspend) {
            driverUSDHC->Disable();
        }
        if (err != kStatus_Success) {
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
        if (!mmcCard->isHostReady) {
            return media_status::uninit;
        }
        if ((mmcCard->csd.flags & kMMC_CsdPermanentWriteProtectFlag) ||
            (mmcCard->csd.flags & kMMC_CsdTemporaryWriteProtectFlag)) {
            return media_status::wprotect;
        }
        return media_status::healthly;
    }

    auto disk_emmc::get_info(info_type what, hwpart_t hwpart) const -> scount_t
    {
        cpp_freertos::LockGuard lock(mutex);
        switch (what) {
        case info_type::sector_size:
            return mmcCard->blockSize;
        case info_type::sector_count:
            switch (hwpart) {
            case kMMC_AccessPartitionUserArea:
                return mmcCard->userPartitionBlocks;
            case kMMC_AccessPartitionBoot1:
            case kMMC_AccessPartitionBoot2:
                return mmcCard->bootPartitionBlocks;
            default:
                return mmcCard->systemPartitionBlocks;
            }
        case info_type::erase_block:
            // not supported
            return 0;
        case info_type::start_sector:
            // not supported
            return 0;
        }
        return -ENOTSUP;
    }

    auto disk_emmc::switch_partition(hwpart_t newpart) -> int
    {
        if (newpart > kMMC_AccessGeneralPurposePartition4) {
            return -ERANGE;
        }

        int ret{};
        if (newpart != currHwPart) {
            if (pmState == pm_state::suspend) {
                driverUSDHC->Enable();
            }
            ret = MMC_SelectPartition(mmcCard.get(), static_cast<mmc_access_partition_t>(newpart));
            if (pmState == pm_state::suspend) {
                driverUSDHC->Disable();
            }
            if (ret == kStatus_Success) {
                currHwPart = newpart;
            }
            else {
                LOG_ERROR("Unable to switch partition err %i", ret);
            }
        }
        return ret;
    }

    auto disk_emmc::pm_control(pm_state target_state) -> int
    {
        if (pmState != target_state) {
            cpp_freertos::LockGuard lock(mutex);
            if (target_state == pm_state::suspend) {
                driverUSDHC->Disable();
            }
            else {
                driverUSDHC->Enable();
            }

            pmState = target_state;
        }

        return kStatus_Success;
    }

    auto disk_emmc::pm_read(pm_state &current_state) -> int
    {
        current_state = pmState;
        return kStatus_Success;
    }

    auto disk_emmc::get_emmc_manufacturer() const -> std::string
    {
        const std::uint8_t manufacturerId = mmcCard->cid.manufacturerID;
        const auto manufacturer           = mmcManufacturersMap.find(manufacturerId);
        if (manufacturer != mmcManufacturersMap.end()) {
            return manufacturer->second;
        }
        return "unknown (0x" + utils::byteToHex(manufacturerId) + ")";
    }

    auto disk_emmc::get_emmc_info() const -> disk_emmc_info
    {
        disk_emmc_info emmcInfo{};
        emmcInfo.manufacturer = get_emmc_manufacturer();
        emmcInfo.blocksCount  = mmcCard->userPartitionBlocks; // Not to be confused with ext4 partition labeled 'user'!
        emmcInfo.blockSize    = mmcCard->blockSize;
        emmcInfo.capacity =
            static_cast<std::uint64_t>(emmcInfo.blocksCount) * static_cast<std::uint64_t>(emmcInfo.blockSize);
        emmcInfo.name =
            std::string(reinterpret_cast<const char *>(mmcCard->cid.productName), sizeof(mmcCard->cid.productName));
        emmcInfo.versionMajor    = (mmcCard->cid.productVersion >> 4) & 0x0F;
        emmcInfo.versionMinor    = mmcCard->cid.productVersion & 0x0F;
        emmcInfo.serialNumber    = mmcCard->cid.productSerialNumber;
        emmcInfo.productionMonth = (mmcCard->cid.manufacturerData >> 4) & 0x0F;

        /* See JESD84-B51, p.163, Table 77 */
        emmcInfo.productionYear = mmcCard->cid.manufacturerData & 0x0F;
        if ((mmcCard->extendedCsd.extendecCsdVersion > 4) && (emmcInfo.productionYear <= 0b1100)) {
            emmcInfo.productionYear += 2013;
        }
        else {
            emmcInfo.productionYear += 1997;
        }

        return emmcInfo;
    }

    auto disk_emmc::get_emmc_info_str() const -> std::string
    {
        constexpr auto bytesPerGB  = 1000UL * 1000UL * 1000UL;
        constexpr auto bytesPerGiB = 1024UL * 1024UL * 1024UL;

        const disk_emmc_info emmcInfo = get_emmc_info();
        const auto capacityGB         = static_cast<float>(emmcInfo.capacity) / static_cast<float>(bytesPerGB);
        const auto capacityGiB        = static_cast<float>(emmcInfo.capacity) / static_cast<float>(bytesPerGiB);

        std::stringstream ss;
        ss << "\t> Manufacturer: " << emmcInfo.manufacturer << std::endl;
        ss << "\t> Blocks count: " << emmcInfo.blocksCount << std::endl;
        ss << "\t> Block size: " << emmcInfo.blockSize << std::endl;
        ss << "\t> Capacity: " << std::fixed << std::setprecision(2) << capacityGB << "GB/" << capacityGiB << "GiB"
           << std::endl;
        ss << "\t> Name: " << emmcInfo.name << std::endl;
        ss << "\t> Version: " << std::to_string(emmcInfo.versionMajor) << "." << std::to_string(emmcInfo.versionMinor)
           << std::endl;
        ss << "\t> SN: " << emmcInfo.serialNumber << std::endl;
        ss << "\t> Production date: " << std::setfill('0') << std::setw(2) << std::to_string(emmcInfo.productionMonth)
           << "/" << emmcInfo.productionYear;

        return ss.str();
    }
} // namespace purefs::blkdev
