/*
 *  @file rt1051_audiocodec.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_RT1051AUDIOCODEC_HPP
#define PUREPHONE_RT1051AUDIOCODEC_HPP


#include "bsp/audio/bsp_audio.hpp"
#include "fsl_sai_edma.h"
#include "FreeRTOS.h"
#include "task.h"
#include "macros.h"

#include "CodecMAX98090.hpp"

#include "drivers/DriverInterface.hpp"
#include "drivers/pll/DriverPLL.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"

namespace bsp{


    void txAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
    void rxAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
    void inAudioCodecWorkerTask(void* pvp);
    void outAudioCodecWorkerTask(void* pvp);

    class RT1051Audiocodec : public AudioDevice{

    public:

        friend void txAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        friend void rxAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        friend void inAudioCodecWorkerTask(void* pvp);
        friend void outAudioCodecWorkerTask(void* pvp);

        RT1051Audiocodec(AudioDevice::audioCallback_t callback);
        virtual ~RT1051Audiocodec();

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
        CodecParamsMAX98090 codecParams;
        CodecMAX98090 codec;

        // M.P: It is important to destroy these drivers in specific order
        std::shared_ptr<drivers::DriverPLL> pll;
        std::shared_ptr<drivers::DriverDMAMux> dmamux;
        std::shared_ptr<drivers::DriverDMA> dma;



        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle);
        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t rxHandle);

        // CODEC_CHANNEL_PCM_BUFFER_SIZE * 2 for double buffering
        static ALIGN_(4) int16_t inBuffer[CODEC_CHANNEL_PCM_BUFFER_SIZE*2];

        // CODEC_CHANNEL_PCM_BUFFER_SIZE * 2 for double buffering
        static ALIGN_(4) int16_t outBuffer[CODEC_CHANNEL_PCM_BUFFER_SIZE*2];

        void Init();
        void Deinit();
        void OutStart();
        void InStart();
        void OutStop();
        void InStop();


    };
}


#endif //PUREPHONE_RT1051AUDIOCODEC_HPP
