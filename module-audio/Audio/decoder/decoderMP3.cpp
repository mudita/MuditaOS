// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO

#include "decoderMP3.hpp"
#include <cstdio>

namespace audio
{
    decoderMP3::decoderMP3(const std::string &filePath) : Decoder(filePath)
    {
        if (fileSize == 0) {
            return;
        }

        mp3 = std::make_unique<drmp3>();

        drmp3_init(mp3.get(), drmp3_read, drmp3_seek, this, nullptr);
        if (mp3 == nullptr) {
            LOG_ERROR("Unable to initialize MP3 decoder");
            return;
        }

        chanNumber = mp3->channels;
        sampleRate = mp3->sampleRate;
        // NOTE: Always convert to S16LE as internal format
        bitsPerSample = 16;
        isInitialized = true;
    }

    decoderMP3::~decoderMP3()
    {
        drmp3_uninit(mp3.get());
    }
    void decoderMP3::setPosition(float pos)
    {
        if (!isInitialized) {
            LOG_ERROR("MP3 decoder not initialized");
            return;
        }
        auto totalFramesCount = drmp3_get_pcm_frame_count(mp3.get());
        drmp3_seek_to_pcm_frame(mp3.get(), totalFramesCount * pos);
        position = static_cast<float>(totalFramesCount) * pos / static_cast<float>(sampleRate);
    }

    std::uint32_t decoderMP3::decode(std::uint32_t samplesToRead, std::int16_t *pcmData)
    {
        std::uint32_t samplesRead =
            drmp3_read_pcm_frames_s16(mp3.get(), samplesToRead / chanNumber, reinterpret_cast<drmp3_int16 *>(pcmData));
        if (samplesRead > 0) {
            position += static_cast<float>(samplesRead) / static_cast<float>(sampleRate);
        }

        return samplesRead * chanNumber;
    }

    size_t decoderMP3::drmp3_read(void *pUserData, void *pBufferOut, size_t bytesToRead)
    {
        const auto decoderContext = reinterpret_cast<decoderMP3 *>(pUserData);
        return std::fread(pBufferOut, 1, bytesToRead, decoderContext->fd);
    }

    drmp3_bool32 decoderMP3::drmp3_seek(void *pUserData, int offset, drmp3_seek_origin origin)
    {
        const auto decoderContext = reinterpret_cast<decoderMP3 *>(pUserData);
        const int seekError =
            std::fseek(decoderContext->fd, offset, origin == drmp3_seek_origin_start ? SEEK_SET : SEEK_CUR);
        return (seekError == 0) ? DRMP3_TRUE : DRMP3_FALSE;
    }

} // namespace audio
