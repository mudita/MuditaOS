// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO

#include "decoderMP3.hpp"

#include <array>
#include <cstdio>

namespace audio
{

    decoderMP3::decoderMP3(const char *fileName) : Decoder(fileName)
    {

        if (fileSize == 0) {
            return;
        }

        mp3 = std::make_unique<drmp3>();

        drmp3_init(mp3.get(), this->drmp3_read, this->drmp3_seek, this, NULL);
        if (mp3 == NULL) {
            LOG_ERROR("********************************** Unable to init mp3 decoder");
            return;
            // Failed to open file
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
        auto totalFramesCount = drmp3_get_pcm_frame_count(mp3.get());
        drmp3_seek_to_pcm_frame(mp3.get(), totalFramesCount * pos);
        position = float(totalFramesCount) * pos / float(sampleRate);
    }

    uint32_t decoderMP3::decode(uint32_t samplesToRead, int16_t *pcmData)
    {

        uint32_t samplesRead = 0;
        samplesRead =
            drmp3_read_pcm_frames_s16(mp3.get(), samplesToRead / chanNumber, reinterpret_cast<drmp3_int16 *>(pcmData));

        if (samplesRead) {
            position += float(samplesRead) / float(sampleRate);
        }

        return samplesRead * chanNumber;
    }

    size_t decoderMP3::drmp3_read(void *pUserData, void *pBufferOut, size_t bytesToRead)
    {
        auto *userdata = reinterpret_cast<decoderMP3 *>(pUserData);
        return std::fread(pBufferOut, 1, bytesToRead, userdata->fd);
    }

    drmp3_bool32 decoderMP3::drmp3_seek(void *pUserData, int offset, drmp3_seek_origin origin)
    {
        auto *userdata = reinterpret_cast<decoderMP3 *>(pUserData);
        return !std::fseek(userdata->fd, offset, origin == drmp3_seek_origin_start ? SEEK_SET : SEEK_CUR);
    }

} // namespace audio
