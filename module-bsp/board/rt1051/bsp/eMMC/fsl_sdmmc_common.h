/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FSL_SDMMC_COMMON_H_
#define _FSL_SDMMC_COMMON_H_

#include "fsl_common.h"
#include "fsl_sdmmc_host.h"
#include "fsl_sdmmc_spec.h"
#include "stdlib.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Middleware version. */
#define FSL_SDMMC_DRIVER_VERSION (MAKE_VERSION(2U, 2U, 4U)) /*2.2.4*/

/*! @brief Reverse byte sequence in uint32_t */
#define SWAP_WORD_BYTE_SEQUENCE(x) (__REV(x))
/*! @brief Reverse byte sequence for each half word in uint32_t */
#define SWAP_HALF_WROD_BYTE_SEQUENCE(x) (__REV16(x))
/*! @brief Maximum loop count to check the card operation voltage range */
#define FSL_SDMMC_MAX_VOLTAGE_RETRIES (1000U)
/*! @brief Maximum loop count to send the cmd */
#define FSL_SDMMC_MAX_CMD_RETRIES (10U)
/*! @brief Default block size */
#define FSL_SDMMC_DEFAULT_BLOCK_SIZE (512U)
/*! @brief SDMMC global data buffer size, word unit*/
#define SDMMC_GLOBAL_BUFFER_SIZE (128U)

/* Common definition for cache line size align */
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
#if defined(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
#if defined(FSL_FEATURE_L2DCACHE_LINESIZE_BYTE)
#define SDMMC_DATA_BUFFER_ALIGN_CACHE MAX(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE, FSL_FEATURE_L2DCACHE_LINESIZE_BYTE)
#else
#define SDMMC_DATA_BUFFER_ALIGN_CACHE FSL_FEATURE_L1DCACHE_LINESIZE_BYTE
#endif
#else
#define SDMMC_DATA_BUFFER_ALIGN_CACHE 1
#endif
#else
#define SDMMC_DATA_BUFFER_ALIGN_CACHE 1
#endif

/*! @brief SD/MMC card API's running status. */
enum _sdmmc_status
{
    kStatus_SDMMC_NotSupportYet             = MAKE_STATUS(kStatusGroup_SDMMC, 0U),  /*!< Haven't supported */
    kStatus_SDMMC_TransferFailed            = MAKE_STATUS(kStatusGroup_SDMMC, 1U),  /*!< Send command failed */
    kStatus_SDMMC_SetCardBlockSizeFailed    = MAKE_STATUS(kStatusGroup_SDMMC, 2U),  /*!< Set block size failed */
    kStatus_SDMMC_HostNotSupport            = MAKE_STATUS(kStatusGroup_SDMMC, 3U),  /*!< Host doesn't support */
    kStatus_SDMMC_CardNotSupport            = MAKE_STATUS(kStatusGroup_SDMMC, 4U),  /*!< Card doesn't support */
    kStatus_SDMMC_AllSendCidFailed          = MAKE_STATUS(kStatusGroup_SDMMC, 5U),  /*!< Send CID failed */
    kStatus_SDMMC_SendRelativeAddressFailed = MAKE_STATUS(kStatusGroup_SDMMC, 6U),  /*!< Send relative address failed */
    kStatus_SDMMC_SendCsdFailed             = MAKE_STATUS(kStatusGroup_SDMMC, 7U),  /*!< Send CSD failed */
    kStatus_SDMMC_SelectCardFailed          = MAKE_STATUS(kStatusGroup_SDMMC, 8U),  /*!< Select card failed */
    kStatus_SDMMC_SendScrFailed             = MAKE_STATUS(kStatusGroup_SDMMC, 9U),  /*!< Send SCR failed */
    kStatus_SDMMC_SetDataBusWidthFailed     = MAKE_STATUS(kStatusGroup_SDMMC, 10U), /*!< Set bus width failed */
    kStatus_SDMMC_GoIdleFailed              = MAKE_STATUS(kStatusGroup_SDMMC, 11U), /*!< Go idle failed */
    kStatus_SDMMC_HandShakeOperationConditionFailed =
        MAKE_STATUS(kStatusGroup_SDMMC, 12U), /*!< Send Operation Condition failed */
    kStatus_SDMMC_SendApplicationCommandFailed =
        MAKE_STATUS(kStatusGroup_SDMMC, 13U), /*!< Send application command failed */
    kStatus_SDMMC_SwitchFailed               = MAKE_STATUS(kStatusGroup_SDMMC, 14U), /*!< Switch command failed */
    kStatus_SDMMC_StopTransmissionFailed     = MAKE_STATUS(kStatusGroup_SDMMC, 15U), /*!< Stop transmission failed */
    kStatus_SDMMC_WaitWriteCompleteFailed    = MAKE_STATUS(kStatusGroup_SDMMC, 16U), /*!< Wait write complete failed */
    kStatus_SDMMC_SetBlockCountFailed        = MAKE_STATUS(kStatusGroup_SDMMC, 17U), /*!< Set block count failed */
    kStatus_SDMMC_SetRelativeAddressFailed   = MAKE_STATUS(kStatusGroup_SDMMC, 18U), /*!< Set relative address failed */
    kStatus_SDMMC_SwitchBusTimingFailed      = MAKE_STATUS(kStatusGroup_SDMMC, 19U), /*!< Switch high speed failed */
    kStatus_SDMMC_SendExtendedCsdFailed      = MAKE_STATUS(kStatusGroup_SDMMC, 20U), /*!< Send EXT_CSD failed */
    kStatus_SDMMC_ConfigureBootFailed        = MAKE_STATUS(kStatusGroup_SDMMC, 21U), /*!< Configure boot failed */
    kStatus_SDMMC_ConfigureExtendedCsdFailed = MAKE_STATUS(kStatusGroup_SDMMC, 22U), /*!< Configure EXT_CSD failed */
    kStatus_SDMMC_EnableHighCapacityEraseFailed =
        MAKE_STATUS(kStatusGroup_SDMMC, 23U), /*!< Enable high capacity erase failed */
    kStatus_SDMMC_SendTestPatternFailed    = MAKE_STATUS(kStatusGroup_SDMMC, 24U), /*!< Send test pattern failed */
    kStatus_SDMMC_ReceiveTestPatternFailed = MAKE_STATUS(kStatusGroup_SDMMC, 25U), /*!< Receive test pattern failed */
    kStatus_SDMMC_SDIO_ResponseError       = MAKE_STATUS(kStatusGroup_SDMMC, 26U), /*!< sdio response error */
    kStatus_SDMMC_SDIO_InvalidArgument =
        MAKE_STATUS(kStatusGroup_SDMMC, 27U), /*!< sdio invalid argument response error */
    kStatus_SDMMC_SDIO_SendOperationConditionFail =
        MAKE_STATUS(kStatusGroup_SDMMC, 28U), /*!< sdio send operation condition fail */
    kStatus_SDMMC_InvalidVoltage           = MAKE_STATUS(kStatusGroup_SDMMC, 29U), /*!<  invaild voltage */
    kStatus_SDMMC_SDIO_SwitchHighSpeedFail = MAKE_STATUS(kStatusGroup_SDMMC, 30U), /*!<  switch to high speed fail */
    kStatus_SDMMC_SDIO_ReadCISFail         = MAKE_STATUS(kStatusGroup_SDMMC, 31U), /*!<  read CIS fail */
    kStatus_SDMMC_SDIO_InvalidCard         = MAKE_STATUS(kStatusGroup_SDMMC, 32U), /*!<  invaild SDIO card */
    kStatus_SDMMC_TuningFail               = MAKE_STATUS(kStatusGroup_SDMMC, 33U), /*!<  tuning fail */
    kStatus_SDMMC_SwitchVoltageFail        = MAKE_STATUS(kStatusGroup_SDMMC, 34U), /*!< switch voltage fail*/
    kStatus_SDMMC_ReTuningRequest          = MAKE_STATUS(kStatusGroup_SDMMC, 35U), /*!<  retuning request */
    kStatus_SDMMC_SetDriverStrengthFail    = MAKE_STATUS(kStatusGroup_SDMMC, 36U), /*!<  set driver strength fail */
    kStatus_SDMMC_SetPowerClassFail        = MAKE_STATUS(kStatusGroup_SDMMC, 37U), /*!<  set power class fail */
    kStatus_SDMMC_HostNotReady             = MAKE_STATUS(kStatusGroup_SDMMC, 38U), /*!<  host controller not ready */
    kStatus_SDMMC_CardDetectFailed         = MAKE_STATUS(kStatusGroup_SDMMC, 39U), /*!<  card detect failed */
    kStatus_SDMMC_PartitioningFailed       = MAKE_STATUS(kStatusGroup_SDMMC, 40U), /*!<  Partitioning failed */
    kStatus_SDMMC_PartitioningNotSupported = MAKE_STATUS(kStatusGroup_SDMMC, 41U), /*!<  Partitioning not supported */
};

/*! @brief card operation voltage */
typedef enum _sdmmc_operation_voltage
{
    kCARD_OperationVoltageNone = 0U, /*!< indicate current voltage setting is not setting bu suser*/
    kCARD_OperationVoltage330V = 1U, /*!< card operation voltage around 3.3v */
    kCARD_OperationVoltage300V = 2U, /*!< card operation voltage around 3.0v */
    kCARD_OperationVoltage180V = 3U, /*!< card operation voltage around 31.8v */
} sdmmc_operation_voltage_t;

/*************************************************************************************************
 * API
 ************************************************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif

    /*!
     * @brief Selects the card to put it into transfer state.
     *
     * @param base SDMMCHOST peripheral base address.
     * @param transfer SDMMCHOST transfer function.
     * @param relativeAddress Relative address.
     * @param isSelected True to put card into transfer state.
     * @retval kStatus_SDMMC_TransferFailed Transfer failed.
     * @retval kStatus_Success Operate successfully.
     */
    status_t SDMMC_SelectCard(SDMMCHOST_TYPE *base,
                              SDMMCHOST_TRANSFER_FUNCTION transfer,
                              uint32_t relativeAddress,
                              bool isSelected);

    /*!
     * @brief Sends an application command.
     *
     * @param base SDMMCHOST peripheral base address.
     * @param transfer SDMMCHOST transfer function.
     * @param relativeAddress Card relative address.
     * @retval kStatus_SDMMC_TransferFailed Transfer failed.
     * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
     * @retval kStatus_Success Operate successfully.
     */
    status_t SDMMC_SendApplicationCommand(SDMMCHOST_TYPE *base,
                                          SDMMCHOST_TRANSFER_FUNCTION transfer,
                                          uint32_t relativeAddress);

    /*!
     * @brief Sets the block count.
     *
     * @param base SDMMCHOST peripheral base address.
     * @param transfer SDMMCHOST transfer function.
     * @param blockCount Block count.
     * @retval kStatus_SDMMC_TransferFailed Transfer failed.
     * @retval kStatus_Success Operate successfully.
     */
    status_t SDMMC_SetBlockCount(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer, uint32_t blockCount);

    /*!
     * @brief Sets the card to be idle state.
     *
     * @param base SDMMCHOST peripheral base address.
     * @param transfer SDMMCHOST transfer function.
     * @retval kStatus_SDMMC_TransferFailed Transfer failed.
     * @retval kStatus_Success Operate successfully.
     */
    status_t SDMMC_GoIdle(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer);

    /*!
     * @brief Sets data block size.
     *
     * @param base SDMMCHOST peripheral base address.
     * @param transfer SDMMCHOST transfer function.
     * @param blockSize Block size.
     * @retval kStatus_SDMMC_TransferFailed Transfer failed.
     * @retval kStatus_Success Operate successfully.
     */
    status_t SDMMC_SetBlockSize(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer, uint32_t blockSize);

    /*!
     * @brief Sets card to inactive status
     *
     * @param base SDMMCHOST peripheral base address.
     * @param transfer SDMMCHOST transfer function.
     * @retval kStatus_SDMMC_TransferFailed Transfer failed.
     * @retval kStatus_Success Operate successfully.
     */
    status_t SDMMC_SetCardInactive(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer);

    /*!
     * @brief provide a simple delay function for sdmmc
     *
     * @param num Delay num*10000.
     */
    void SDMMC_Delay(uint32_t num);

    /*!
     * @brief provide a voltage switch function for SD/SDIO card
     *
     * @param base SDMMCHOST peripheral base address.
     * @param transfer SDMMCHOST transfer function.
     */
    status_t SDMMC_SwitchVoltage(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer);

    /*!
     * @brief excute tuning
     *
     * @param base SDMMCHOST peripheral base address.
     * @param transfer Host transfer function
     * @param tuningCmd Tuning cmd
     * @param blockSize Tuning block size
     */
    status_t SDMMC_ExecuteTuning(SDMMCHOST_TYPE *base,
                                 SDMMCHOST_TRANSFER_FUNCTION transfer,
                                 uint32_t tuningCmd,
                                 uint32_t blockSize);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_SDMMC_COMMON_H_ */
