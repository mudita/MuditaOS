// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051CELLULARAUDIO_HPP
#define PUREPHONE_RT1051CELLULARAUDIO_HPP

#include "SAIAudioDevice.hpp"
#include "fsl_sai_edma.h"

#include "FreeRTOS.h"
#include "task.h"
#include <board/macros.h>

#include "drivers/pll/DriverPLL.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"

#include <vector>

namespace audio
{

    void txCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
    void rxCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

    class RT1051CellularAudio : public SAIAudioDevice
    {

      public:
        friend void txCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        friend void rxCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

        RT1051CellularAudio();
        virtual ~RT1051CellularAudio();

        AudioDevice::RetCode Start() override final;
        AudioDevice::RetCode Stop() override final;

        AudioDevice::RetCode setOutputVolume(float vol) override final;
        AudioDevice::RetCode setInputGain(float gain) override final;
        auto getSupportedFormats() -> std::vector<AudioFormat> override final;
        auto getTraits() const -> Traits override final;
        auto getSourceFormat() -> AudioFormat override final;

      private:
        static constexpr auto supportedSampleRate = 16000U;
        static constexpr auto supportedBitWidth   = 16U;
        static constexpr auto supportedChannels   = 1U;

        enum class State
        {
            Running,
            Stopped
        };

        State state                = State::Stopped;
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
} // namespace audio

#endif // PUREPHONE_RT1051CELLULARAUDIO_HPP
