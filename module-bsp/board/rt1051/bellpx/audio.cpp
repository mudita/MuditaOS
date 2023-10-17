// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "audio.hpp"

#include "board/BoardDefinitions.hpp"

extern "C"
{
#include "board.h"
#include "dma_config.h"
}

using namespace bsp::audio;
using namespace drivers;

AudioConfig audioConfig;

namespace
{
    constexpr bool isDerivativeOf44100(const std::uint32_t sampleRate)
    {
        return sampleRate % 11025 == 0;
    }

    DriverPLLParams getPLLParams(const std::uint32_t sampleRate)
    {
        /// The basic formula for calculating PLL4:
        /// (fsclk * (loopDivider + numerator/denominator)) / postDivider

        if (isDerivativeOf44100(sampleRate)) {
            /// PLL4 set to 722.5344MHz, later it will be divided by SAI to generate 11,2896MHz BCLK
            return {
                .loopDivider = 30,
                .postDivider = 1,
                .numerator   = 1056,
                .denominator = 10000,
                .src         = 0,
            };
        }
        else {
            /// PLL4 set to 786.48MHz, later it will be divided by SAI to generate 12,28875MHz BCLK
            return {
                .loopDivider = 32,
                .postDivider = 1,
                .numerator   = 77,
                .denominator = 100,
                .src         = 0,
            };
        }
    }
} // namespace

void bsp::audio::init(const std::uint32_t sampleRate)
{
    audioConfig.pllAudio =
        DriverPLL::Create(static_cast<PLLInstances>(BoardDefinitions::AUDIO_PLL), getPLLParams(sampleRate));
    audioConfig.dmamux =
        DriverDMAMux::Create(static_cast<DMAMuxInstances>(BoardDefinitions::AUDIOCODEC_DMAMUX), DriverDMAMuxParams{});
    audioConfig.dma = DriverDMA::Create(static_cast<DMAInstances>(BoardDefinitions::AUDIOCODEC_DMA), DriverDMAParams{});

    // Enable MCLK clock
    IOMUXC_GPR->GPR1 |= BELL_AUDIOCODEC_SAIx_MCLK_MASK;

    audioConfig.txDMAHandle =
        audioConfig.dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::AUDIOCODEC_TX_DMA_CHANNEL));
    audioConfig.rxDMAHandle =
        audioConfig.dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::AUDIOCODEC_RX_DMA_CHANNEL));
    audioConfig.dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::AUDIOCODEC_TX_DMA_CHANNEL),
                               BSP_AUDIOCODEC_SAIx_DMA_TX_SOURCE);
    audioConfig.dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::AUDIOCODEC_RX_DMA_CHANNEL),
                               BSP_AUDIOCODEC_SAIx_DMA_RX_SOURCE);

    audioConfig.mclkSourceClockHz = GetPerphSourceClock(PerphClock_SAI1);

    // Initialize SAI Tx module
    SAI_TxGetDefaultConfig(&audioConfig.config);
    audioConfig.config.masterSlave = kSAI_Master;
    SAI_TxInit(BELL_AUDIOCODEC_SAIx, &audioConfig.config);

    // Initialize SAI Rx module
    SAI_RxGetDefaultConfig(&audioConfig.config);

    audioConfig.config.masterSlave = kSAI_Master;
    SAI_RxInit(BELL_AUDIOCODEC_SAIx, &audioConfig.config);
}

void bsp::audio::deinit()
{
    memset(&audioConfig.config, 0, sizeof(audioConfig.config));
    SAI_Deinit(BELL_AUDIOCODEC_SAIx);
    if (audioConfig.dmamux) {
        audioConfig.dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::AUDIOCODEC_TX_DMA_CHANNEL));
        audioConfig.dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::AUDIOCODEC_RX_DMA_CHANNEL));
    }

    // force order of destruction
    audioConfig.txDMAHandle.reset();
    audioConfig.rxDMAHandle.reset();
    audioConfig.dma.reset();
    audioConfig.dmamux.reset();
    audioConfig.pllAudio.reset();
}

AudioConfig *bsp::audio::AudioConfig::get()
{
    return &audioConfig;
}
