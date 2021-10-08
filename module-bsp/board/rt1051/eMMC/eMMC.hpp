// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_BSP_EMMC_HPP
#define MODULE_BSP_EMMC_HPP

#include <stdint.h>
#include <memory>

#if defined(TARGET_RT1051)
#include "board/rt1051/bsp/eMMC/fsl_mmc.h"
#include "drivers/pll/DriverPLL.hpp"
#else
#error "Unsupported target"
#endif

#include "bsp/common.hpp"
#include "drivers/usdhc/DriverUSDHC.hpp"

namespace bsp
{
    class eMMC
    {
      public:
        enum class Partition
        {
            UserArea        = 0U, /*!< No access to boot partition (default), normal partition */
            Boot1           = 1U, /*!< Read/Write boot partition 1 */
            Boot2           = 2U, /*!< Read/Write boot partition 2*/
            RPMB            = 3U, /*!< Replay protected mem block */
            GeneralPurpose1 = 4U, /*!< access to general purpose partition 1 */
            GeneralPurpose2 = 5U, /*!< access to general purpose partition 2 */
            GeneralPurpose3 = 6U, /*!< access to general purpose partition 3 */
            GeneralPurpose4 = 7U, /*!< access to general purpose partition 4 */

        };

        eMMC()
        {}
        ~eMMC()
        {}

        RetCode Init();
        RetCode DeInit();
        RetCode ReadBlocks(uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);
        RetCode WriteBlocks(const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);
        RetCode SwitchPartition(Partition partition);

        uint32_t userPartitionBlocks;

      protected:
      private:
#if defined(TARGET_RT1051)
        std::shared_ptr<drivers::DriverPLL> pll;
        mmc_card_t mmcCard;
#else
#error "Unsupported target"
#endif
        std::shared_ptr<drivers::DriverUSDHC> driverUSDHC;
    };

} // namespace bsp

#endif // MODULE_BSP_EMMC_HPP
