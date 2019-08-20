/*
 * @file fsl_edma.h
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 29 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_BSP_BOARD_LINUX_FSL_DRIVERS_FSL_EDMA_H_
#define MODULE_BSP_BOARD_LINUX_FSL_DRIVERS_FSL_EDMA_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! @brief Callback for eDMA */
struct _edma_handle;

typedef void (*edma_callback)(struct _edma_handle *handle, void *userData, bool transferDone, uint32_t tcds);

/*! @brief eDMA transfer handle structure */
typedef struct _edma_handle
{
    edma_callback callback; /*!< Callback function for major count exhausted. */
    void *userData;         /*!< Callback function parameter. */
//    DMA_Type *base;         /*!< eDMA peripheral base address. */
//    edma_tcd_t *tcdPool;    /*!< Pointer to memory stored TCDs. */
    uint8_t channel;        /*!< eDMA channel number. */
    volatile int8_t header; /*!< The first TCD index. Should point to the next TCD to be loaded into the eDMA engine. */
    volatile int8_t tail;   /*!< The last TCD index. Should point to the next TCD to be stored into the memory pool. */
    volatile int8_t tcdUsed; /*!< The number of used TCD slots. Should reflect the number of TCDs can be used/loaded in
                                the memory. */
    volatile int8_t tcdSize; /*!< The total number of TCD slots in the queue. */
    uint8_t flags;           /*!< The status of the current channel. */
} edma_handle_t;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MODULE_BSP_BOARD_LINUX_FSL_DRIVERS_FSL_EDMA_H_ */
