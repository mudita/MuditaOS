/*
 *  @file RT1051CellularAudio.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 03.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RT1051CellularAudio.hpp"
#include "board.h"
#include "dma_config.h"

#include "log/log.hpp"

#include "bsp/BoardDefinitions.hpp"


namespace bsp {

    using namespace drivers;

    sai_edma_handle_t RT1051CellularAudio::txHandle = {};
    sai_edma_handle_t RT1051CellularAudio::rxHandle = {};
    int16_t RT1051CellularAudio::inBuffer[CODEC_CHANNEL_PCM_BUFFER_SIZE * 2] = {};
    int16_t RT1051CellularAudio::outBuffer[CODEC_CHANNEL_PCM_BUFFER_SIZE * 2] = {};

    RT1051CellularAudio::RT1051CellularAudio(bsp::AudioDevice::audioCallback_t callback) : AudioDevice(callback),
                                                                                           saiInFormat{},
                                                                                           saiOutFormat{},
                                                                                           config{} {
        isInitialized = true;
    }

    RT1051CellularAudio::~RT1051CellularAudio() {
        Stop();
        Deinit();
    }

    int32_t RT1051CellularAudio::Start(const bsp::AudioDevice::Format &format) {

        Init();

        saiInFormat.bitWidth = format.bitWidth;
        saiInFormat.sampleRate_Hz = format.sampleRate_Hz;

        saiOutFormat.bitWidth = format.bitWidth;
        saiOutFormat.sampleRate_Hz = format.sampleRate_Hz;


        if (format.flags & static_cast<uint32_t >(AudioDevice::Flags::InputLeft)) {
            saiInFormat.stereo = kSAI_MonoLeft;
            InStart();
        } else if (format.flags & static_cast<uint32_t >(AudioDevice::Flags::InputRight)) {
            saiInFormat.stereo = kSAI_MonoRight;
            InStart();
        } else if (format.flags & static_cast<uint32_t >(AudioDevice::Flags::InputStereo)) {
            saiInFormat.stereo = kSAI_Stereo;
            InStart();
        }

        if (format.flags & static_cast<uint32_t >(AudioDevice::Flags::OutputMono)) {
            saiOutFormat.stereo = kSAI_MonoLeft;
            OutStart();
        } else if (format.flags & static_cast<uint32_t >(AudioDevice::Flags::OutPutStereo)) {
            saiOutFormat.stereo = kSAI_Stereo;
            OutStart();
        }

        // Store format passed
        currentFormat = format;

        return 0;
    }

    int32_t RT1051CellularAudio::Stop() {

        InStop();
        OutStop();

        if (outWorkerThread) {
            vTaskDelete(outWorkerThread);
            outWorkerThread = nullptr;
        }
        if (inWorkerThread) {
            vTaskDelete(inWorkerThread);
            inWorkerThread = nullptr;
        }


        currentFormat = {};
        return 0;

    }

    int32_t RT1051CellularAudio::OutputVolumeCtrl(float vol) {
        currentFormat.outputVolume = vol;
        return 0;
    }

    int32_t RT1051CellularAudio::InputGainCtrl(float gain) {
        currentFormat.inputGain = gain;
        return 0;
    }

    int32_t RT1051CellularAudio::InputPathCtrl(InputPath inputPath) {
        currentFormat.inputPath = inputPath;
        return 0;
    }

    int32_t RT1051CellularAudio::OutputPathCtrl(OutputPath outputPath) {
        currentFormat.outputPath = outputPath;
        return 0;
    }

    bool RT1051CellularAudio::IsFormatSupported(const bsp::AudioDevice::Format &format) {
        return true;
    }





    // INTERNALS


    void RT1051CellularAudio::Init() {

        pll = DriverPLL::Create(static_cast<PLLInstances >(BoardDefinitions::AUDIO_PLL),
                                                 DriverPLLParams{});

        dmamux = DriverDMAMux::Create(
                static_cast<DMAMuxInstances >(BoardDefinitions::CELLULAR_AUDIO_DMAMUX), DriverDMAMuxParams{});

        dma = DriverDMA::Create(static_cast<DMAInstances >(BoardDefinitions::CELLULAR_AUDIO_DMA),
                                                 DriverDMAParams{});


        // Enable MCLK clock
        IOMUXC_GPR->GPR1 |= BOARD_CELLULAR_AUDIO_SAIx_MCLK_MASK;

        txDMAHandle = dma->CreateHandle(static_cast<uint32_t >(BoardDefinitions::CELLULAR_AUDIO_TX_DMA_CHANNEL));
        rxDMAHandle = dma->CreateHandle(static_cast<uint32_t >(BoardDefinitions::CELLULAR_AUDIO_RX_DMA_CHANNEL));
        dmamux->Enable(static_cast<uint32_t >(BoardDefinitions::CELLULAR_AUDIO_TX_DMA_CHANNEL),
                       BSP_CELLULAR_AUDIO_SAIx_DMA_TX_SOURCE); // TODO: M.P fix BSP_CELLULAR_AUDIO_SAIx_DMA_TX_SOURCE
        dmamux->Enable(static_cast<uint32_t >(BoardDefinitions::CELLULAR_AUDIO_RX_DMA_CHANNEL),
                       BSP_CELLULAR_AUDIO_SAIx_DMA_RX_SOURCE); // TODO: M.P fix BSP_CELLULAR_AUDIO_SAIx_DMA_RX_SOURCE

        mclkSourceClockHz = BOARD_CELLULAR_AUDIO_SAIx_CLK_FREQ;

        // Initialize SAI Tx module
        SAI_TxGetDefaultConfig(&config);
        config.masterSlave = kSAI_Slave;
        config.protocol = kSAI_BusPCMA;
        SAI_TxInit(BOARD_CELLULAR_AUDIO_SAIx, &config);

        // Initialize SAI Rx module
        SAI_RxGetDefaultConfig(&config);
        config.masterSlave = kSAI_Slave;
        config.protocol = kSAI_BusPCMA;
        SAI_RxInit(BOARD_CELLULAR_AUDIO_SAIx, &config);
    }

    void RT1051CellularAudio::Deinit() {
        memset(&config, 0, sizeof config);
        SAI_Deinit(BOARD_CELLULAR_AUDIO_SAIx);
        dmamux->Disable(static_cast<uint32_t >(BoardDefinitions ::CELLULAR_AUDIO_TX_DMA_CHANNEL));
        dmamux->Disable(static_cast<uint32_t >(BoardDefinitions ::CELLULAR_AUDIO_RX_DMA_CHANNEL));
    }

    void RT1051CellularAudio::InStart() {
        sai_transfer_format_t sai_format = {0};
        sai_transfer_t xfer = {0};

        saiInFormat.data = (uint8_t *) inBuffer;
        saiInFormat.dataSize = CODEC_CHANNEL_PCM_BUFFER_SIZE * saiInFormat.bitWidth / 8;

        /* Configure the audio format */
        sai_format.bitWidth = saiInFormat.bitWidth;
        sai_format.channel = 0U;
        sai_format.sampleRate_Hz = saiInFormat.sampleRate_Hz;
        sai_format.masterClockHz = mclkSourceClockHz;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol = config.protocol;
        sai_format.stereo = saiInFormat.stereo;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferRxCreateHandleEDMA(BOARD_CELLULAR_AUDIO_SAIx, &rxHandle, rxCellularCallback, this,
                                       reinterpret_cast<edma_handle_t *>(rxDMAHandle->GetHandle()));

        SAI_TransferRxSetFormatEDMA(BOARD_CELLULAR_AUDIO_SAIx, &rxHandle, &sai_format, mclkSourceClockHz,
                                    mclkSourceClockHz);

        DisableIRQ(BOARD_CELLULAR_AUDIO_SAIx_RX_IRQ);

        /* Reset SAI Rx internal logic */
        SAI_RxSoftwareReset(BOARD_CELLULAR_AUDIO_SAIx, kSAI_ResetTypeSoftware);

        xfer.data = saiInFormat.data;
        xfer.dataSize = saiInFormat.dataSize;
        SAI_TransferReceiveEDMA(BOARD_CELLULAR_AUDIO_SAIx, &rxHandle, &xfer);

        if (xTaskCreate(inCellularWorkerTask, "incellaudio", 512, this, 0, &inWorkerThread) != pdPASS) {
            LOG_ERROR("Error during creating input cellular audio task");
        }
    }

    void RT1051CellularAudio::OutStart() {
        sai_transfer_format_t sai_format = {0};
        sai_transfer_t xfer = {0};

        saiOutFormat.data = (uint8_t *) outBuffer;
        saiOutFormat.dataSize = CODEC_CHANNEL_PCM_BUFFER_SIZE * saiInFormat.bitWidth / 8;

        /* Configure the audio format */
        sai_format.bitWidth = saiOutFormat.bitWidth;
        sai_format.channel = 0U;
        sai_format.sampleRate_Hz = saiOutFormat.sampleRate_Hz;
        sai_format.masterClockHz = mclkSourceClockHz;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol = config.protocol;
        sai_format.stereo = saiOutFormat.stereo;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferTxCreateHandleEDMA(BOARD_CELLULAR_AUDIO_SAIx, &txHandle, txCellularCallback, this,
                                       reinterpret_cast<edma_handle_t *>(txDMAHandle->GetHandle()));

        SAI_TransferTxSetFormatEDMA(BOARD_CELLULAR_AUDIO_SAIx, &txHandle, &sai_format, mclkSourceClockHz,
                                    mclkSourceClockHz);

        DisableIRQ(BOARD_CELLULAR_AUDIO_SAIx_TX_IRQ);

        /* Reset SAI Tx internal logic */
        SAI_TxSoftwareReset(BOARD_CELLULAR_AUDIO_SAIx, kSAI_ResetTypeSoftware);

        xfer.data = saiOutFormat.data;
        xfer.dataSize = saiOutFormat.dataSize;
        SAI_TransferSendEDMA(BOARD_CELLULAR_AUDIO_SAIx, &txHandle, &xfer);

        if (xTaskCreate(outCellularWorkerTask, "outcellaudio", 512, this, 0, &outWorkerThread) != pdPASS) {
            LOG_ERROR("Error during creating  output cellular audio task");
        }

        //Fill out buffer with data
        GetAudioCallback()(nullptr, outBuffer,
                           RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE * 2);
    }

    void RT1051CellularAudio::OutStop() {
        SAI_TxDisableInterrupts(BOARD_CELLULAR_AUDIO_SAIx, kSAI_FIFOErrorInterruptEnable);
        if (txHandle.dmaHandle) {
            SAI_TransferTerminateSendEDMA(BOARD_CELLULAR_AUDIO_SAIx, &txHandle);
        }
        memset(&txHandle, 0, sizeof(txHandle));
    }

    void RT1051CellularAudio::InStop() {
        SAI_RxDisableInterrupts(BOARD_CELLULAR_AUDIO_SAIx, kSAI_FIFOErrorInterruptEnable);
        if (rxHandle.dmaHandle) {
            SAI_TransferAbortReceiveEDMA(BOARD_CELLULAR_AUDIO_SAIx, &rxHandle);
        }
        memset(&rxHandle, 0, sizeof(rxHandle));
    }


    void inCellularWorkerTask(void *pvp) {
        uint32_t ulNotificationValue = 0;

        RT1051CellularAudio *inst = reinterpret_cast<RT1051CellularAudio *>(pvp);

        while (1) {
            xTaskNotifyWait(0x00,               /* Don't clear any bits on entry. */
                            UINT32_MAX,          /* Clear all bits on exit. */
                            &ulNotificationValue, /* Receives the notification value. */
                            portMAX_DELAY);    /* Block indefinitely. */

            if (ulNotificationValue & static_cast<uint32_t >(RT1051CellularAudio::irq_state_t::IRQStateHalfTransfer)) {
                auto framesFetched = inst->GetAudioCallback()(inst->inBuffer, nullptr,
                                                              RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE);
                if (framesFetched == 0) {
                    goto cleanup;
                } else if (framesFetched < RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE) {
                    memset(&inst->inBuffer[framesFetched], 0,
                           RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE - framesFetched);
                }
            }

            if (ulNotificationValue & static_cast<uint32_t >(RT1051CellularAudio::irq_state_t::IRQStateFullTransfer)) {
                auto framesFetched = inst->GetAudioCallback()(
                        &inst->inBuffer[RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE], nullptr,
                        RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE);

                if (framesFetched == 0) {
                    goto cleanup;
                } else if (framesFetched < RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE) {
                    memset(&inst->inBuffer[RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE + framesFetched], 0,
                           RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE - framesFetched);
                }
            }
        }

        cleanup:
        inst->InStop();
        inst->inWorkerThread = nullptr;
        vTaskDelete(NULL);
    }

    void outCellularWorkerTask(void *pvp) {
        uint32_t ulNotificationValue = 0;

        RT1051CellularAudio *inst = reinterpret_cast<RT1051CellularAudio *>(pvp);

        while (1) {
            xTaskNotifyWait(0x00,               /* Don't clear any bits on entry. */
                            UINT32_MAX,          /* Clear all bits on exit. */
                            &ulNotificationValue, /* Receives the notification value. */
                            portMAX_DELAY);    /* Block indefinitely. */

            if (ulNotificationValue & static_cast<uint32_t >(RT1051CellularAudio::irq_state_t::IRQStateHalfTransfer)) {
                auto framesFetched = inst->GetAudioCallback()(nullptr, inst->outBuffer,
                                                              RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE);
                if (framesFetched == 0) {
                    goto cleanup;
                } else if (framesFetched < RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE) {
                    memset(&inst->outBuffer[framesFetched], 0,
                           RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE - framesFetched);
                }
            }

            if (ulNotificationValue & static_cast<uint32_t >(RT1051CellularAudio::irq_state_t::IRQStateFullTransfer)) {
                auto framesFetched = inst->GetAudioCallback()(nullptr,
                                                              &inst->outBuffer[RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE],
                                                              RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE);

                if (framesFetched == 0) {
                    goto cleanup;
                } else if (framesFetched < RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE) {
                    memset(&inst->outBuffer[RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE + framesFetched], 0,
                           RT1051CellularAudio::CODEC_CHANNEL_PCM_BUFFER_SIZE - framesFetched);
                }
            }
        }

        cleanup:
        inst->OutStop();
        inst->outWorkerThread = nullptr;
        vTaskDelete(NULL);
    }

    void rxCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData) {
        static RT1051CellularAudio::irq_state_t state = RT1051CellularAudio::irq_state_t::IRQStateHalfTransfer;
        RT1051CellularAudio *inst = (RT1051CellularAudio *) userData;
        sai_transfer_t xfer = {0};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (state == RT1051CellularAudio::irq_state_t::IRQStateHalfTransfer) {

            xfer.dataSize = inst->saiInFormat.dataSize;
            xfer.data = inst->saiInFormat.data + (inst->saiInFormat.dataSize);
            SAI_TransferReceiveEDMA(BOARD_CELLULAR_AUDIO_SAIx, &inst->rxHandle, &xfer);

            /* Notify the task that the transmission is complete. */
            if (inst->inWorkerThread) {
                xTaskNotifyFromISR(inst->inWorkerThread, static_cast<uint32_t >(state), eSetBits,
                                   &xHigherPriorityTaskWoken);
            }
            state = RT1051CellularAudio::irq_state_t::IRQStateFullTransfer;

        } else {

            xfer.dataSize = inst->saiInFormat.dataSize;
            xfer.data = inst->saiInFormat.data;
            SAI_TransferReceiveEDMA(BOARD_CELLULAR_AUDIO_SAIx, &inst->rxHandle, &xfer);

            /* Notify the task that the transmission is complete. */
            if (inst->inWorkerThread) {
                xTaskNotifyFromISR(inst->inWorkerThread, static_cast<uint32_t >(state), eSetBits,
                                   &xHigherPriorityTaskWoken);
            }
            state = RT1051CellularAudio::irq_state_t::IRQStateHalfTransfer;


        }


        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    void txCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData) {
        static RT1051CellularAudio::irq_state_t state = RT1051CellularAudio::irq_state_t::IRQStateHalfTransfer;
        RT1051CellularAudio *inst = (RT1051CellularAudio *) userData;
        sai_transfer_t xfer = {0};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (state == RT1051CellularAudio::irq_state_t::IRQStateHalfTransfer) {

            xfer.dataSize = inst->saiOutFormat.dataSize;
            xfer.data = inst->saiOutFormat.data + (inst->saiOutFormat.dataSize);
            SAI_TransferSendEDMA(BOARD_CELLULAR_AUDIO_SAIx, &inst->txHandle, &xfer);

            /* Notify the task that the transmission is complete. */
            if (inst->outWorkerThread) {
                xTaskNotifyFromISR(inst->outWorkerThread, static_cast<uint32_t >(state), eSetBits,
                                   &xHigherPriorityTaskWoken);
            }
            state = RT1051CellularAudio::irq_state_t::IRQStateFullTransfer;


        } else {
            xfer.dataSize = inst->saiOutFormat.dataSize;
            xfer.data = inst->saiOutFormat.data;
            SAI_TransferSendEDMA(BOARD_CELLULAR_AUDIO_SAIx, &inst->txHandle, &xfer);

            /* Notify the task that the transmission is complete. */
            if (inst->outWorkerThread) {
                xTaskNotifyFromISR(inst->outWorkerThread, static_cast<uint32_t >(state), eSetBits,
                                   &xHigherPriorityTaskWoken);
            }
            state = RT1051CellularAudio::irq_state_t::IRQStateHalfTransfer;

        }

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

}