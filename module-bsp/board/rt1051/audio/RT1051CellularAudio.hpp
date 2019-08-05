/*
 *  @file RT1051CellularAudio.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 03.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_RT1051CELLULARAUDIO_HPP
#define PUREPHONE_RT1051CELLULARAUDIO_HPP

#include "audio/bsp_audio.hpp"
#include "fsl_sai_edma.h"
#include "FreeRTOS.h"
#include "task.h"
#include "macros.h"

namespace bsp{


    void txCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
    void rxCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
    void inCellularWorkerTask(void* pvp);
    void outCellularWorkerTask(void* pvp);

    class RT1051CellularAudio : public AudioDevice{

    public:

        friend void txCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        friend void rxCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        friend void inCellularWorkerTask(void* pvp);
        friend void outCellularWorkerTask(void* pvp);

        RT1051CellularAudio(AudioDevice::audioCallback_t callback);
        virtual ~RT1051CellularAudio();

        int32_t Start(const Format& format) override final;
        int32_t Stop() override final;
        int32_t OutputVolumeCtrl(float vol) override final;
        int32_t InputGainCtrl(float gain) override final;
        int32_t OutputPathCtrl(OutputPath outputPath) override final;
        int32_t InputPathCtrl(InputPath inputPath) override final;
        bool IsFormatSupported(const Format& format) override final;

    private:

        static const uint32_t CODEC_CHANNEL_PCM_BUFFER_SIZE=1024;

        /*! @brief Internals state of Rx/Tx callback, needed for double buffering technique */
        enum class irq_state_t
        {
            IRQStateHalfTransfer=1<<0,
            IRQStateFullTransfer=1<<1
        };

        struct SAIFormat{
            uint32_t sampleRate_Hz;   /*!< Sample rate of audio data */
            uint32_t bitWidth;        /*!< Data length of audio data, usually 8/16/24/32 bits */
            sai_mono_stereo_t stereo; /*!< Mono or stereo */
            uint8_t *data;   /*!< Data start address to transfer. */
            size_t dataSize; /*!< Transfer size. */
        };

        SAIFormat saiInFormat;
        SAIFormat saiOutFormat;
        uint32_t mclkSourceClockHz = 0;
        sai_config_t config;
        TaskHandle_t inWorkerThread = nullptr;
        TaskHandle_t outWorkerThread = nullptr;


        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle);
        static edma_handle_t dmaTxHandle;
        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t rxHandle);
        static edma_handle_t dmaRxHandle;

        // CODEC_CHANNEL_PCM_BUFFER_SIZE * 2 for double buffering
        static ALIGN_(4) int16_t inBuffer[CODEC_CHANNEL_PCM_BUFFER_SIZE*2];

        // CODEC_CHANNEL_PCM_BUFFER_SIZE * 2 for double buffering
        static ALIGN_(4) int16_t outBuffer[CODEC_CHANNEL_PCM_BUFFER_SIZE*2];

        void Init();
        void Deinit();
        void PLLInit();
        void OutStart();
        void InStart();
        void OutStop();
        void InStop();


    };
}


#endif //PUREPHONE_RT1051CELLULARAUDIO_HPP
