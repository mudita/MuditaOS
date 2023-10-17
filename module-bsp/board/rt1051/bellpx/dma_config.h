// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_BSP_BOARD_RT1051_EINK_DMA_CONFIG_H_
#define MODULE_BSP_BOARD_RT1051_EINK_DMA_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *   **********************************************************************************************************************
 *   * *
 *   *                                              DMA FOR EINK *
 *   * *
 *   **********************************************************************************************************************
 */

/**
 *   Eink SPI TX DMA config
 */
#define BSP_EINK_LPSPI_DMA_TX_DMAMUX_BASE DMAMUX
#define BSP_EINK_LPSPI_DMA_TX_DMA_BASE    DMA0
#define BSP_EINK_LPSPI_DMA_TX_PERI_SEL    (uint8_t) kDmaRequestMuxLPSPI1Tx
#define BSP_EINK_LPSPI_DMA_TX_CH          0

/**
 *   Eink SPI RX DMA config
 */
#define BSP_EINK_LPSPI_DMA_RX_DMAMUX_BASE DMAMUX
#define BSP_EINK_LPSPI_DMA_RX_DMA_BASE    DMA0
#define BSP_EINK_LPSPI_DMA_RX_PERI_SEL    (uint8_t) kDmaRequestMuxLPSPI1Rx
#define BSP_EINK_LPSPI_DMA_RX_CH          1

    /*
     *   **********************************************************************************************************************
     *   * *
     *   *                                              DMA FOR CELLULAR *
     *   * *
     *   **********************************************************************************************************************
     */

    /* DMA */

#define BSP_CELLULAR_AUDIO_SAIx_DMA_TX_SOURCE kDmaRequestMuxSai1Tx
#define BSP_CELLULAR_AUDIO_SAIx_DMA_RX_SOURCE kDmaRequestMuxSai1Rx

/*
 *   **********************************************************************************************************************
 *   * *
 *   *                                              DMA FOR AUDIOCODEC *
 *   * *
 *   **********************************************************************************************************************
 */
#define BSP_AUDIOCODEC_SAIx_DMA_TX_SOURCE kDmaRequestMuxSai1Tx
#define BSP_AUDIOCODEC_SAIx_DMA_RX_SOURCE kDmaRequestMuxSai1Rx

/*
 *   **********************************************************************************************************************
 *   * *
 *   *                                              DMA FOR BLUETOOTH *
 *   * *
 *   **********************************************************************************************************************
 */
#define BSP_BLUETOOTH_UART_TX_DMA_DMAMUX_BASE DMAMUX
#define BSP_BLUETOOTH_UART_TX_DMA_BASE        DMA0
#define BSP_BLUETOOTH_UART_TX_DMA_CH          8

    /**
     * @brief This function initializes the entire DMA peripheral
     */
    void BOARD_InitDMA();

#ifdef __cplusplus
}
#endif

#endif /* MODULE_BSP_BOARD_RT1051_EINK_DMA_CONFIG_H_ */
