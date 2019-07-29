/*
 *  @file rt1051_audiocodec.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "rt1051_audiocodec.hpp"
#include "board.h"
#include "dma_config.h"
#include "fsl_dmamux.h"



namespace bsp{

    sai_edma_handle_t RT1051Audiocodec::txHandle = {};
    sai_edma_handle_t RT1051Audiocodec::rxHandle = {};
    edma_handle_t dmaTxHandle = {};
    static edma_handle_t dmaRxHandle = {};


    RT1051Audiocodec::RT1051Audiocodec(bsp::AudioDevice::audioCallback_t callback): AudioDevice(callback),config{}{

    }

    RT1051Audiocodec::~RT1051Audiocodec() {

    }


    void RT1051Audiocodec::Init() {
        edma_config_t dmaConfig = {0};

        bsp_audiocodec_audio_pll_Init();

        // Enable MCLK clock
        IOMUXC_GPR->GPR1 |= BOARD_AUDIOCODEC_SAIx_MCLK_MASK;

        EDMA_GetDefaultConfig(&dmaConfig);
        EDMA_Init(BSP_AUDIOCODEC_SAIx_DMA, &dmaConfig);
        DMAMUX_Init(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE);


        /* Create EDMA handle */
        /*
         * dmaConfig.enableRoundRobinArbitration = false;
         * dmaConfig.enableHaltOnError = true;
         * dmaConfig.enableContinuousLinkMode = false;
         * dmaConfig.enableDebugMode = false;
         */
        EDMA_CreateHandle(&dmaTxHandle, BSP_AUDIOCODEC_SAIx_DMA, BSP_AUDIOCODEC_SAIx_DMA_TX_CHANNEL);
        EDMA_CreateHandle(&dmaRxHandle, BSP_AUDIOCODEC_SAIx_DMA, BSP_AUDIOCODEC_SAIx_DMA_RX_CHANNEL);

        DMAMUX_SetSource(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE, BSP_AUDIOCODEC_SAIx_DMA_TX_CHANNEL, (uint8_t)BSP_AUDIOCODEC_SAIx_DMA_TX_SOURCE);
        DMAMUX_EnableChannel(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE, BSP_AUDIOCODEC_SAIx_DMA_TX_CHANNEL);

        DMAMUX_SetSource(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE, BSP_AUDIOCODEC_SAIx_DMA_RX_CHANNEL, (uint8_t)BSP_AUDIOCODEC_SAIx_DMA_RX_SOURCE);
        DMAMUX_EnableChannel(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE,BSP_AUDIOCODEC_SAIx_DMA_RX_CHANNEL);


        mclkSourceClockHz = BOARD_AUDIOCODEC_SAIx_CLK_FREQ;

        // Initialize SAI Tx module
        SAI_TxGetDefaultConfig(&config);
        config.masterSlave = kSAI_Slave;
        SAI_TxInit(BOARD_AUDIOCODEC_SAIx, &config);

        // Initialize SAI Rx module
        SAI_RxGetDefaultConfig(&config);

        config.masterSlave = kSAI_Slave;
        SAI_RxInit(BOARD_AUDIOCODEC_SAIx, &config);
    }

    void RT1051Audiocodec::Deinit() {
        current_state = Idle;
        memset(&config,0,sizeof config);
        //CLOCK_DeinitAudioPll();
        //DMAMUX_Deinit(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE);
        //EDMA_Deinit(BSP_AUDIOCODEC_SAIx_DMA);
        SAI_Deinit(BOARD_AUDIOCODEC_SAIx);
    }

    void RT1051Audiocodec::rxCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData) {

    }

    void RT1051Audiocodec::txCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData) {
        static irq_state_t state = irq_state_t ::IRQStateHalfTransfer;
        bsp_audiocodec_format_t* fmt = (bsp_audiocodec_format_t*)userData;
        sai_transfer_t xfer = {0};

        if(state == irq_state_t ::IRQStateHalfTransfer){

            xfer.dataSize = fmt->dataSize/2;
            xfer.data = fmt->data + (fmt->dataSize/2);
            SAI_TransferSendEDMA(BOARD_AUDIOCODEC_SAIx, &txHandle, &xfer);

            state = irq_state_t ::IRQStateFullTransfer;
            bsp_audiocodec_out_HalfTransfer_CallBack();

        }
        else{

            xfer.dataSize = fmt->dataSize/2;
            xfer.data = fmt->data;
            SAI_TransferSendEDMA(BOARD_AUDIOCODEC_SAIx, &txHandle, &xfer);

            state = irq_state_t ::IRQStateHalfTransfer;
            bsp_audiocodec_out_FullTransfer_CallBack();
            xfer.data = fmt->data;
        }
    }

}