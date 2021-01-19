// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051CELLULARAUDIO_HPP
#define PUREPHONE_RT1051CELLULARAUDIO_HPP

#include "SAIAudioDevice.hpp"
#include "fsl_sai_edma.h"

#include "FreeRTOS.h"
#include "task.h"
#include "macros.h"

#include "drivers/pll/DriverPLL.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"

#include <mutex.hpp>

namespace bsp
{

    void txCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
    void rxCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

    class RT1051CellularAudio : public SAIAudioDevice
    {

      public:
        friend void txCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        friend void rxCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

        RT1051CellularAudio(AudioDevice::audioCallback_t callback);
        virtual ~RT1051CellularAudio();

        AudioDevice::RetCode Start(const Format &format) override final;
        AudioDevice::RetCode Stop() override final;
        AudioDevice::RetCode OutputVolumeCtrl(float vol) override final;
        AudioDevice::RetCode InputGainCtrl(float gain) override final;
        AudioDevice::RetCode OutputPathCtrl(OutputPath outputPath) override final;
        AudioDevice::RetCode InputPathCtrl(InputPath inputPath) override final;
        bool IsFormatSupported(const Format &format) override final;

        cpp_freertos::MutexStandard mutex;

      private:
        enum class State
        {
            Running,
            Stopped
        };

        struct SAIFormat
        {
            uint32_t sampleRate_Hz;   /*!< Sample rate of audio data */
            uint32_t bitWidth;        /*!< Data length of audio data, usually 8/16/24/32 bits */
            sai_mono_stereo_t stereo; /*!< Mono or stereo */
        };

        State state = State::Stopped;
        SAIFormat saiInFormat;
        SAIFormat saiOutFormat;
        uint32_t mclkSourceClockHz = 0;
        sai_config_t config;

        // M.P: It is important to destroy these drivers in specific order
        std::shared_ptr<drivers::DriverPLL> pll;
        std::shared_ptr<drivers::DriverDMAMux> dmamux;
        std::shared_ptr<drivers::DriverDMA> dma;
        std::unique_ptr<drivers::DriverDMAHandle> rxDMAHandle;
        std::unique_ptr<drivers::DriverDMAHandle> txDMAHandle;

        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle);
        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t rxHandle);

        void Init();
        void Deinit();
        void OutStart();
        void InStart();
        void OutStop();
        void InStop();
    };
} // namespace bsp

#endif // PUREPHONE_RT1051CELLULARAUDIO_HPP
