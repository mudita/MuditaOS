/*
 * @file dma_config.h
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_BSP_BOARD_RT1051_EINK_DMA_CONFIG_H_
#define MODULE_BSP_BOARD_RT1051_EINK_DMA_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *   **********************************************************************************************************************
 *   *                                                                                                                    *
 *   *                                              DMA FOR EINK                                                          *
 *   *                                                                                                                    *
 *   **********************************************************************************************************************
 */

/**
 *   Eink SPI TX DMA config
 */
#define BSP_EINK_LPSPI_DMA_TX_DMAMUX_BASE               DMAMUX
#define BSP_EINK_LPSPI_DMA_TX_DMA_BASE                  DMA0
#define BSP_EINK_LPSPI_DMA_TX_PERI_SEL                  (uint8_t) kDmaRequestMuxLPSPI1Tx
#define BSP_EINK_LPSPI_DMA_TX_CH                        0

/**
 *   Eink SPI RX DMA config
 */
#define BSP_EINK_LPSPI_DMA_RX_DMAMUX_BASE               DMAMUX
#define BSP_EINK_LPSPI_DMA_RX_DMA_BASE                  DMA0
#define BSP_EINK_LPSPI_DMA_RX_PERI_SEL                  (uint8_t) kDmaRequestMuxLPSPI1Rx
#define BSP_EINK_LPSPI_DMA_RX_CH                        1

/**
 *   Eink INTERNAL MEMORY COPYING DMA config
 */
#define BSP_EINK_MEMCPY_DMA_DMAMUX_BASE                 DMAMUX
#define BSP_EINK_MEMCPY_DMA_DMA_BASE                    DMA0
#define BSP_EINK_MEMCPY_DMA_CH                          2

/*
 *   **********************************************************************************************************************
 *   *                                                                                                                    *
 *   *                                              DMA FOR CELLULAR                                                      *
 *   *                                                                                                                    *
 *   **********************************************************************************************************************
 */

/* DMA */
#define BSP_CELLULAR_AUDIO_SAIx_DMA                     DMA0
#define BSP_CELLULAR_AUDIO_SAIx_DMAMUX_BASE             DMAMUX
#define BSP_CELLULAR_AUDIO_SAIx_DMA_TX_CHANNEL              (3U)
#define BSP_CELLULAR_AUDIO_SAIx_DMA_RX_CHANNEL              (4U)
#define BSP_CELLULAR_AUDIO_SAIx_DMA_TX_SOURCE	            kDmaRequestMuxSai1Tx
#define BSP_CELLULAR_AUDIO_SAIx_DMA_RX_SOURCE         	kDmaRequestMuxSai1Rx

#define BSP_CELLULAR_UART_TX_DMA_DMAMUX_BASE            DMAMUX
#define BSP_CELLULAR_UART_TX_DMA_BASE                   DMA0
#define BSP_CELLULAR_UART_TX_DMA_CH                     5

/*
 *   **********************************************************************************************************************
 *   *                                                                                                                    *
 *   *                                              DMA FOR AUDIOCODEC                                                    *
 *   *                                                                                                                    *
 *   **********************************************************************************************************************
 */

#define BSP_AUDIOCODEC_SAIx_DMA                     DMA0
#define BSP_AUDIOCODEC_SAIx_DMAMUX_BASE             DMAMUX
#define BSP_AUDIOCODEC_SAIx_DMA_TX_CHANNEL              (6U)
#define BSP_AUDIOCODEC_SAIx_DMA_RX_CHANNEL              (7U)
#define BSP_AUDIOCODEC_SAIx_DMA_TX_SOURCE	kDmaRequestMuxSai2Tx
#define BSP_AUDIOCODEC_SAIx_DMA_RX_SOURCE	kDmaRequestMuxSai2Rx

/*
 *   **********************************************************************************************************************
 *   *                                                                                                                    *
 *   *                                              DMA FOR BLUETOOTH                                                     *
 *   *                                                                                                                    *
 *   **********************************************************************************************************************
 */
#define BSP_BLUETOOTH_UART_TX_DMA_DMAMUX_BASE           DMAMUX
#define BSP_BLUETOOTH_UART_TX_DMA_BASE                  DMA0
#define BSP_BLUETOOTH_UART_TX_DMA_CH                    8

/**
 * @brief This function initializes the entire DMA peripheral
 */
void BOARD_InitDMA();

#ifdef __cplusplus
}
#endif

#endif /* MODULE_BSP_BOARD_RT1051_EINK_DMA_CONFIG_H_ */
