// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DecoderWAV.hpp"
#include "DecoderCommon.hpp"
#include <log/log.hpp>
#include <memory>

#define DR_WAV_IMPLEMENTATION
#include <src/dr_wav.h>

namespace audio
{
    DecoderWAV::DecoderWAV(const std::string &filePath) : Decoder(filePath), wav(std::make_unique<drwav>())
    {
        if (drwav_init_file(wav.get(), filePath.c_str(), nullptr) == DRWAV_FALSE) {
            LOG_ERROR("Unable to init WAV decoder");
            return;
        }

        /* NOTE: Always convert to S16LE as an internal format */
        channelCount  = wav->channels;
        sampleRate    = wav->sampleRate;
        bitsPerSample = 16;
        isInitialized = true;
    }

    DecoderWAV::~DecoderWAV()
    {
        if (isInitialized) {
            drwav_uninit(wav.get());
        }
    }

    std::int32_t DecoderWAV::decode(std::uint32_t samplesToRead, std::int16_t *pcmData)
    {
        if (!isInitialized) {
            LOG_ERROR("WAV decoder not initialized");
            return 0;
        }

        if (!fileExists(fd)) {
            LOG_WARN("File '%s' was deleted during playback!", filePath.c_str());
            return fileDeletedRetCode;
        }

        const auto samplesRead = drwav_read_pcm_frames_s16(wav.get(), samplesToRead / channelCount, pcmData);
        if (samplesRead > 0) {
            /* Calculate frame duration in seconds */
            position += static_cast<float>(samplesRead) / static_cast<float>(sampleRate);
        }
        return samplesRead * channelCount;
    }

    void DecoderWAV::setPosition(float pos)
    {
        if (!isInitialized) {
            LOG_ERROR("WAV decoder not initialized");
            return;
        }
        drwav_seek_to_pcm_frame(wav.get(), wav->totalPCMFrameCount * pos);

        /* Calculate new position */
        position = static_cast<float>(wav->totalPCMFrameCount) * pos / static_cast<float>(sampleRate);
    }
} // namespace audio
