// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "decoderWAV.hpp"
#include <log/log.hpp>
#include <memory>
#include "Audio/AudioCommon.hpp"

#define DR_WAV_IMPLEMENTATION
#include <src/dr_wav.h>

namespace audio
{
    namespace internal
    {
        struct wavContext
        {
            drwav wav;
        };
    } // namespace internal

    decoderWAV::decoderWAV(const char *fileName)
        : Decoder(fileName), decoderContext(std::make_unique<internal::wavContext>())
    {
        auto dwav = &decoderContext->wav;
        if (!drwav_init_file(dwav, fileName, NULL)) {
            LOG_ERROR("Unable to init wav decoder");
            return;
        }
        sampleRate = dwav->sampleRate;
        // NOTE: Always convert to S16LE as internal format
        bitsPerSample = 16;
        // Number of channels
        chanNumber    = dwav->channels;
        isInitialized = true;
    }

    decoderWAV::~decoderWAV()
    {
        if (isInitialized) {
            auto dwav = &decoderContext->wav;
            drwav_uninit(dwav);
        }
    }

    uint32_t decoderWAV::decode(uint32_t samplesToRead, int16_t *pcmData)
    {
        if (!isInitialized) {
            LOG_ERROR("Wav decoder not initialized");
            return 0;
        }
        auto dwav               = &decoderContext->wav;
        const auto samples_read = drwav_read_pcm_frames_s16(dwav, samplesToRead / chanNumber, pcmData);
        if (samples_read) {
            /* Calculate frame duration in seconds */
            position += float(samplesToRead) / float(sampleRate);
        }
        return samples_read * chanNumber;
    }

    void decoderWAV::setPosition(float pos)
    {
        if (!isInitialized) {
            LOG_ERROR("Wav decoder not initialized");
            return;
        }
        auto dwav = &decoderContext->wav;
        drwav_seek_to_pcm_frame(dwav, dwav->totalPCMFrameCount * pos);
    }

    auto decoderWAV::getBitWidth() -> unsigned int
    {
        return bitsPerSample;
    }

} // namespace audio
