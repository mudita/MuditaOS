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

#include "log/log.hpp"


namespace bsp {

    sai_edma_handle_t RT1051Audiocodec::txHandle = {};
    sai_edma_handle_t RT1051Audiocodec::rxHandle = {};
    edma_handle_t dmaTxHandle = {};
    static edma_handle_t dmaRxHandle = {};


    RT1051Audiocodec::RT1051Audiocodec(bsp::AudioDevice::audioCallback_t callback) : AudioDevice(callback), config{} {

    }

    RT1051Audiocodec::~RT1051Audiocodec() {

    }

    int32_t RT1051Audiocodec::Start(const bsp::AudioDevice::Format &format) {

        uint32_t outChan = 0;
        uint32_t inChan = 0;

        SAIFormat fmt = {};
        fmt.bitWidth = format.bitWidth;
        fmt.sampleRate_Hz = format.sampleRate_Hz;


        if (format.flags & static_cast<uint32_t >(AudioDevice::Flags::InputLeft)) {
            inChan++;
        }

        if (format.flags & static_cast<uint32_t >(AudioDevice::Flags::InputRight)) {
            inChan++;
        }

        if (format.flags & static_cast<uint32_t >(AudioDevice::Flags::OutputMono)) {
            outChan++;
        }

        if (format.flags & static_cast<uint32_t >(AudioDevice::Flags::OutPutStereo)) {
            outChan = 2;
        }

        // Full duplex mono or full duplex stereo
        if ((inChan == 1 && outChan == 1) || (inChan == 2 && outChan == 2)) {
            InStart();
            OutStart();
        }
            // In stereo or mono
        else if ((inChan == 2 || inChan == 1) && outChan == 0) {
            InStart();
        }
            // Out stereo or mono
        else if ((outChan == 2 || outChan == 1) && inChan == 0) {
            OutStart();
        } else {
            // not supported
            LOG_ERROR("Format configuration not supported");
        }

        if (xTaskCreate(inWorkerTask, "inaudiocodec", 1024, this, 0, &inWorkerThread) != pdPASS) {
            LOG_ERROR("Error during creating input audiocodec task");
        }

        if (xTaskCreate(outWorkerTask, "outaudiocodec", 1024, this, 0, &outWorkerThread) != pdPASS) {
            LOG_ERROR("Error during creating  output audiocodec task");
        }


    }






    // INTERNALS


    void RT1051Audiocodec::Init() {
        edma_config_t dmaConfig = {0};

        PLLInit();

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

        DMAMUX_SetSource(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE, BSP_AUDIOCODEC_SAIx_DMA_TX_CHANNEL,
                         (uint8_t) BSP_AUDIOCODEC_SAIx_DMA_TX_SOURCE);
        DMAMUX_EnableChannel(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE, BSP_AUDIOCODEC_SAIx_DMA_TX_CHANNEL);

        DMAMUX_SetSource(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE, BSP_AUDIOCODEC_SAIx_DMA_RX_CHANNEL,
                         (uint8_t) BSP_AUDIOCODEC_SAIx_DMA_RX_SOURCE);
        DMAMUX_EnableChannel(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE, BSP_AUDIOCODEC_SAIx_DMA_RX_CHANNEL);


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
        memset(&config, 0, sizeof config);
        //CLOCK_DeinitAudioPll();
        //DMAMUX_Deinit(BSP_AUDIOCODEC_SAIx_DMAMUX_BASE);
        //EDMA_Deinit(BSP_AUDIOCODEC_SAIx_DMA);
        SAI_Deinit(BOARD_AUDIOCODEC_SAIx);
    }

    void RT1051Audiocodec::PLLInit() {
        if (CLOCK_GetPllFreq(kCLOCK_PllAudio) != 0) {
            // Audio PLL initiaized already, skip initialization process
        } else {
            /*
             * AUDIO PLL setting: Frequency = Fref * (DIV_SELECT + NUM / DENOM)
             *                              = 24 * (32 + 77/100)
             *                              = 786.48 MHz
             */
            const clock_audio_pll_config_t audioPllConfig = {
                    .loopDivider = 32,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
                    .postDivider = 1,   /* Divider after the PLL, should only be 1, 2, 4, 8, 16. */
                    .numerator = 77,    /* 30 bit numerator of fractional loop divider. */
                    .denominator = 100, /* 30 bit denominator of fractional loop divider */
            };

            CLOCK_InitAudioPll(&audioPllConfig);
        }
    }

    void RT1051Audiocodec::InStart() {
        sai_transfer_format_t sai_format = {0};
        sai_transfer_t xfer = {0};

        /* Configure the audio format */
        sai_format.bitWidth = saiFormat.bitWidth;
        sai_format.channel = 0U;
        sai_format.sampleRate_Hz = saiFormat.sampleRate_Hz;
        sai_format.masterClockHz = mclkSourceClockHz;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol = config.protocol;
        sai_format.stereo = saiFormat.stereo;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferRxCreateHandleEDMA(BOARD_AUDIOCODEC_SAIx, &rxHandle, rxCallback, &saiFormat, &dmaRxHandle);

        SAI_TransferRxSetFormatEDMA(BOARD_AUDIOCODEC_SAIx, &rxHandle, &sai_format, mclkSourceClockHz,
                                    mclkSourceClockHz);

        DisableIRQ(BOARD_AUDIOCODEC_SAIx_RX_IRQ);

        /* Reset SAI Rx internal logic */
        SAI_RxSoftwareReset(BOARD_AUDIOCODEC_SAIx, kSAI_ResetTypeSoftware);

        xfer.data = saiFormat.data;
        xfer.dataSize = saiFormat.dataSize / 2;
        SAI_TransferReceiveEDMA(BOARD_AUDIOCODEC_SAIx, &rxHandle, &xfer);
    }

    void RT1051Audiocodec::OutStart() {
        sai_transfer_format_t sai_format = {0};
        sai_transfer_t xfer = {0};

        /* Configure the audio format */
        sai_format.bitWidth = saiFormat.bitWidth;
        sai_format.channel = 0U;
        sai_format.sampleRate_Hz = saiFormat.sampleRate_Hz;
        sai_format.masterClockHz = mclkSourceClockHz;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol = config.protocol;
        sai_format.stereo = saiFormat.stereo;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferTxCreateHandleEDMA(BOARD_AUDIOCODEC_SAIx, &txHandle, txCallback, &saiFormat, &dmaTxHandle);

        SAI_TransferTxSetFormatEDMA(BOARD_AUDIOCODEC_SAIx, &txHandle, &sai_format, mclkSourceClockHz,
                                    mclkSourceClockHz);

        DisableIRQ(BOARD_AUDIOCODEC_SAIx_TX_IRQ);

        /* Reset SAI Tx internal logic */
        SAI_TxSoftwareReset(BOARD_AUDIOCODEC_SAIx, kSAI_ResetTypeSoftware);

        xfer.data = saiFormat.data;
        xfer.dataSize = saiFormat.dataSize / 2;
        SAI_TransferSendEDMA(BOARD_AUDIOCODEC_SAIx, &txHandle, &xfer);
    }

    void RT1051Audiocodec::OutStop() {
        SAI_TxDisableInterrupts(BOARD_AUDIOCODEC_SAIx, kSAI_FIFOErrorInterruptEnable);
        SAI_TransferTerminateSendEDMA(BOARD_AUDIOCODEC_SAIx, &txHandle);
    }

    void RT1051Audiocodec::InStop() {
        SAI_RxDisableInterrupts(BOARD_AUDIOCODEC_SAIx, kSAI_FIFOErrorInterruptEnable);
        SAI_TransferAbortReceiveEDMA(BOARD_AUDIOCODEC_SAIx, &rxHandle);
    }


    void inWorkerTask(void *pvp) {
        uint32_t ulNotificationValue = 0;

        RT1051Audiocodec *inst = reinterpret_cast<RT1051Audiocodec *>(pvp);

        while (1) {
            xTaskNotifyWait(0x00,               /* Don't clear any bits on entry. */
                            UINT32_MAX,          /* Clear all bits on exit. */
                            &ulNotificationValue, /* Receives the notification value. */
                            portMAX_DELAY);    /* Block indefinitely. */

            if (ulNotificationValue & static_cast<uint32_t >(RT1051Audiocodec::irq_state_t::IRQStateHalfTransfer)) {
                if (inst->GetAudioCallback()(inst->inBuffer, nullptr,
                                             RT1051Audiocodec::CODEC_CHANNEL_PCM_BUFFER_SIZE)) {
                    goto cleanup;
                }
            }

            if (ulNotificationValue & static_cast<uint32_t >(RT1051Audiocodec::irq_state_t::IRQStateFullTransfer)) {
                if (inst->GetAudioCallback()(&inst->inBuffer[RT1051Audiocodec::CODEC_CHANNEL_PCM_BUFFER_SIZE], nullptr,
                                             RT1051Audiocodec::CODEC_CHANNEL_PCM_BUFFER_SIZE)) {
                    goto cleanup;
                }
            }
        }

        cleanup:
        inst->InStop();
        vTaskDelete(NULL);
    }

    void outWorkerTask(void *pvp) {
        uint32_t ulNotificationValue = 0;

        RT1051Audiocodec *inst = reinterpret_cast<RT1051Audiocodec *>(pvp);

        while (1) {
            xTaskNotifyWait(0x00,               /* Don't clear any bits on entry. */
                            UINT32_MAX,          /* Clear all bits on exit. */
                            &ulNotificationValue, /* Receives the notification value. */
                            portMAX_DELAY);    /* Block indefinitely. */

            if (ulNotificationValue & static_cast<uint32_t >(RT1051Audiocodec::irq_state_t::IRQStateHalfTransfer)) {
                if (inst->GetAudioCallback()(nullptr, inst->outBuffer,
                                             RT1051Audiocodec::CODEC_CHANNEL_PCM_BUFFER_SIZE)) {
                    goto cleanup;
                }
            }

            if (ulNotificationValue & static_cast<uint32_t >(RT1051Audiocodec::irq_state_t::IRQStateFullTransfer)) {
                if (inst->GetAudioCallback()(nullptr,&inst->outBuffer[RT1051Audiocodec::CODEC_CHANNEL_PCM_BUFFER_SIZE],
                                             RT1051Audiocodec::CODEC_CHANNEL_PCM_BUFFER_SIZE)) {
                    goto cleanup;
                }
            }
        }

        cleanup:
        inst->InStop();
        vTaskDelete(NULL);
    }

    void rxCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData) {
        static RT1051Audiocodec::irq_state_t state = RT1051Audiocodec::irq_state_t::IRQStateHalfTransfer;
        RT1051Audiocodec *inst = (RT1051Audiocodec *) userData;
        sai_transfer_t xfer = {0};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (state == RT1051Audiocodec::irq_state_t::IRQStateHalfTransfer) {

            xfer.dataSize = inst->saiFormat.dataSize / 2;
            xfer.data = inst->saiFormat.data + (inst->saiFormat.dataSize / 2);
            SAI_TransferReceiveEDMA(BOARD_AUDIOCODEC_SAIx, &inst->rxHandle, &xfer);

            /* Notify the task that the transmission is complete. */
            if (inst->inWorkerThread) {
                xTaskNotifyFromISR(inst->inWorkerThread, static_cast<uint32_t >(state), eSetBits,
                                   &xHigherPriorityTaskWoken);
            }
            state = RT1051Audiocodec::irq_state_t::IRQStateFullTransfer;

        } else {

            xfer.dataSize = inst->saiFormat.dataSize / 2;
            xfer.data = inst->saiFormat.data;
            SAI_TransferReceiveEDMA(BOARD_AUDIOCODEC_SAIx, &inst->rxHandle, &xfer);

            /* Notify the task that the transmission is complete. */
            if (inst->inWorkerThread) {
                xTaskNotifyFromISR(inst->inWorkerThread, static_cast<uint32_t >(state), eSetBits,
                                   &xHigherPriorityTaskWoken);
            }
            state = RT1051Audiocodec::irq_state_t::IRQStateHalfTransfer;


        }


        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    void txCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData) {
        static RT1051Audiocodec::irq_state_t state = RT1051Audiocodec::irq_state_t::IRQStateHalfTransfer;
        RT1051Audiocodec *inst = (RT1051Audiocodec *) userData;
        sai_transfer_t xfer = {0};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (state == RT1051Audiocodec::irq_state_t::IRQStateHalfTransfer) {

            xfer.dataSize = inst->saiFormat.dataSize / 2;
            xfer.data = inst->saiFormat.data + (inst->saiFormat.dataSize / 2);
            SAI_TransferSendEDMA(BOARD_AUDIOCODEC_SAIx, &inst->txHandle, &xfer);

            /* Notify the task that the transmission is complete. */
            if (inst->outWorkerThread) {
                xTaskNotifyFromISR(inst->outWorkerThread, static_cast<uint32_t >(state), eSetBits,
                                   &xHigherPriorityTaskWoken);
            }
            state = RT1051Audiocodec::irq_state_t::IRQStateFullTransfer;


        } else {
            xfer.dataSize = inst->saiFormat.dataSize / 2;
            xfer.data = inst->saiFormat.data;
            SAI_TransferSendEDMA(BOARD_AUDIOCODEC_SAIx, &inst->txHandle, &xfer);

            /* Notify the task that the transmission is complete. */
            if (inst->outWorkerThread) {
                xTaskNotifyFromISR(inst->outWorkerThread, static_cast<uint32_t >(state), eSetBits,
                                   &xHigherPriorityTaskWoken);
            }
            state = RT1051Audiocodec::irq_state_t::IRQStateHalfTransfer;

        }
    }

}