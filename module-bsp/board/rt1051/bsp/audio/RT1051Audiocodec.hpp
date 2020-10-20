#pragma once

#include "bsp/audio/bsp_audio.hpp"
#include "fsl_sai_edma.h"
#include "FreeRTOS.h"
#include "task.h"
#include "macros.h"

#include "CodecMAX98090.hpp"

#include "drivers/pll/DriverPLL.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"

#include <mutex.hpp>

namespace bsp
{

    void txAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
    void rxAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
    void inAudioCodecWorkerTask(void *pvp);
    void outAudioCodecWorkerTask(void *pvp);

    class RT1051Audiocodec : public AudioDevice
    {

      public:
        static void Init();
        static void Deinit();

        friend void txAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        friend void rxAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        friend void inAudioCodecWorkerTask(void *pvp);
        friend void outAudioCodecWorkerTask(void *pvp);

        RT1051Audiocodec(AudioDevice::audioCallback_t callback);
        virtual ~RT1051Audiocodec();

        AudioDevice::RetCode Start(const Format &format) override final;
        AudioDevice::RetCode Stop() override final;
        AudioDevice::RetCode OutputVolumeCtrl(float vol) override final;
        AudioDevice::RetCode InputGainCtrl(float gain) override final;
        AudioDevice::RetCode OutputPathCtrl(OutputPath outputPath) override final;
        AudioDevice::RetCode InputPathCtrl(InputPath inputPath) override final;
        bool IsFormatSupported(const Format &format) override final;

        cpp_freertos::MutexStandard mutex;

      private:
        static const uint32_t CODEC_CHANNEL_PCM_BUFFER_SIZE = 1024;
        const static TickType_t codecSettleTime             = 20 * portTICK_PERIOD_MS;

        enum class State
        {
            Running,
            Stopped
        };

        /*! @brief Internals state of Rx/Tx callback, needed for double buffering technique */
        enum class TransferState
        {
            HalfTransfer = 1 << 0,
            FullTransfer = 1 << 1,
            Close        = 1 << 2,
        };

        struct SAIFormat
        {
            uint32_t sampleRate_Hz;   /*!< Sample rate of audio data */
            uint32_t bitWidth;        /*!< Data length of audio data, usually 8/16/24/32 bits */
            sai_mono_stereo_t stereo; /*!< Mono or stereo */
            uint8_t *data;            /*!< Data start address to transfer. */
            size_t dataSize;          /*!< Transfer size. */
        };

        static sai_config_t config;
        static std::uint32_t mclkSourceClockHz;

        State state = State::Stopped;
        SAIFormat saiInFormat;
        SAIFormat saiOutFormat;
        TaskHandle_t inWorkerThread  = nullptr;
        TaskHandle_t outWorkerThread = nullptr;
        CodecParamsMAX98090 codecParams;
        CodecMAX98090 codec;

        // M.P: It is important to destroy these drivers in specific order
        static std::shared_ptr<drivers::DriverPLL> pllAudio;
        static std::shared_ptr<drivers::DriverDMAMux> dmamux;
        static std::shared_ptr<drivers::DriverDMA> dma;
        static std::unique_ptr<drivers::DriverDMAHandle> rxDMAHandle;
        static std::unique_ptr<drivers::DriverDMAHandle> txDMAHandle;

        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle);
        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t rxHandle);

        // CODEC_CHANNEL_PCM_BUFFER_SIZE * 2 for double buffering
        static ALIGN_(4) int16_t inBuffer[CODEC_CHANNEL_PCM_BUFFER_SIZE * 2];

        // CODEC_CHANNEL_PCM_BUFFER_SIZE * 2 for double buffering
        static ALIGN_(4) int16_t outBuffer[CODEC_CHANNEL_PCM_BUFFER_SIZE * 2];

        void OutStart();
        void InStart();
        void OutStop();
        void InStop();
    };
} // namespace bsp
