// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "eMMC.hpp"

#include "bsp/BoardDefinitions.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/bsp/eMMC/fsl_mmc.h"
#elif defined(TARGET_Linux)

#else
#error "Unsupported target 1"
#endif

namespace bsp
{

    using namespace drivers;

    RetCode eMMC::Init()
    {
        driverUSDHC = drivers::DriverUSDHC::Create(
            "EMMC", static_cast<drivers::USDHCInstances>(BoardDefinitions::EMMC_USDHC_INSTANCE));
#if defined(TARGET_RT1051)

        // pll = DriverPLL::Create(static_cast<PLLInstances >(BoardDefinitions::EMMC_PLL),DriverPLLParams{});

        mmcCard.busWidth                   = kMMC_DataBusWidth8bit;
        mmcCard.busTiming                  = kMMC_HighSpeedTiming;
        mmcCard.enablePreDefinedBlockCount = true;
        mmcCard.host.base                  = USDHC2; // TODO:M.P move it to BoardDefinitions.hpp
        mmcCard.host.sourceClock_Hz        = GetPerphSourceClock(PerphClock_USDHC2);

        auto ret = MMC_Init(&mmcCard);
        if (ret != kStatus_Success) {
            return RetCode::Failure;
        }
        else {
            userPartitionBlocks = mmcCard.userPartitionBlocks;
            return RetCode::Success;
        }
#else
#error "Unsupported target"
#endif
    }

    RetCode eMMC::DeInit()
    {
#if defined(TARGET_RT1051)
        MMC_Deinit(&mmcCard);
        return RetCode::Success;
#else
#error "Unsupported target"
#endif
    }

    RetCode eMMC::ReadBlocks(uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
    {
#if defined(TARGET_RT1051)
        driverUSDHC->Enable();
        auto ret = MMC_ReadBlocks(&mmcCard, buffer, startBlock, blockCount);
        driverUSDHC->Disable();
        if (ret != kStatus_Success) {
            return RetCode::Failure;
        }
        else {
            return RetCode::Success;
        }
#else
#error "Unsupported target"
#endif
    }

    RetCode eMMC::WriteBlocks(const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
    {
#if defined(TARGET_RT1051)
        driverUSDHC->Enable();
        auto ret = MMC_WriteBlocks(&mmcCard, buffer, startBlock, blockCount);
        driverUSDHC->Disable();
        if (ret != kStatus_Success) {
            return RetCode::Failure;
        }
        else {
            return RetCode::Success;
        }
#else
#error "Unsupported target"
#endif
    }

    RetCode eMMC::SwitchPartition(eMMC::Partition partition)
    {
#if defined(TARGET_RT1051)
        driverUSDHC->Enable();
        auto ret = MMC_SelectPartition(&mmcCard, static_cast<mmc_access_partition_t>(partition));
        driverUSDHC->Disable();
        if (ret != kStatus_Success) {
            return RetCode::Failure;
        }
        else {
            return RetCode::Success;
        }
#else
#error "Unsupported target"
#endif
    }

} // namespace bsp
