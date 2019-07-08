/*
 * @file dma_config.c
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "dma_config.h"

void BOARD_InitDMA()
{
    edma_config_t edmaConfig = {0};

    EDMA_GetDefaultConfig(&edmaConfig);
    EDMA_Init(DMA0, &edmaConfig);
    DMAMUX_Init(DMAMUX);
}

