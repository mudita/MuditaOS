
/*
 * @file eMMC.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 20.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef MODULE_BSP_EMMC_HPP
#define MODULE_BSP_EMMC_HPP

#include <stdint.h>

#if defined(TARGET_RT1051)
#include <eMMC/fsl_mmc.h>
#else
#error "Unsupported target"
#endif


#include "bsp/common.hpp"

namespace bsp
{
    class eMMC
    {
    public:

        enum class Partition{
            UserAera = 0U,                /*!< No access to boot partition (default), normal partition */
            Boot1 = 1U,                   /*!< Read/Write boot partition 1 */
            Boot2 = 2U,                   /*!< Read/Write boot partition 2*/
            RPMB = 3U,                    /*!< Replay protected mem block */
            GeneralPurpose1 = 4U,         /*!< access to general purpose partition 1 */
            GeneralPurpose2 = 5U,         /*!< access to general purpose partition 2 */
            GeneralPurpose3 = 6U,         /*!< access to general purpose partition 3 */
            GeneralPurpose4 = 7U,         /*!< access to general purpose partition 4 */

        };

        eMMC(){}
        ~eMMC(){}

        RetCode Init();
        RetCode DeInit();
        RetCode ReadBlocks(uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);
        RetCode WriteBlocks(const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);
        RetCode SwitchPartition(Partition partition);

        uint32_t userPartitionBlocks;

    protected:

    private:

    #if defined(TARGET_RT1051)
        mmc_card_t mmcCard;
    #else
    #error "Unsupported target"
    #endif

    };

}

#endif //MODULE_BSP_EMMC_HPP
