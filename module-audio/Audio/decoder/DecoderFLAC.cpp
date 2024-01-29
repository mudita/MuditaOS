// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DecoderFLAC.hpp"
#include "DecoderCommon.hpp"
#include "flac/flacfile.h"

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_OGG
#define DR_FLAC_NO_CRC
#define DR_FLAC_NO_SIMD
#include <src/dr_flac.h>

namespace audio
{
    DecoderFLAC::DecoderFLAC(const std::string &filePath) : Decoder(filePath)
    {
        if (fileSize == 0) {
            return;
        }

        flac = drflac_open(drflacRead, drflacSeek, this, nullptr);
        if (flac == nullptr) {
            LOG_ERROR("Unable to initialize FLAC decoder");
            return;
        }

        /* NOTE: Always convert to S16LE as an internal format */
        channelCount  = flac->channels;
        sampleRate    = flac->sampleRate;
        bitsPerSample = 16;
        isInitialized = true;
    }

    DecoderFLAC::~DecoderFLAC()
    {
        drflac_close(flac);
    }

    std::int32_t DecoderFLAC::decode(std::uint32_t samplesToRead, int16_t *pcmData)
    {
        if (!fileExists(fd)) {
            LOG_WARN("File '%s' was deleted during playback!", filePath.c_str());
            return fileDeletedRetCode;
        }

        const auto samplesRead =
            drflac_read_pcm_frames_s16(flac, samplesToRead / channelCount, reinterpret_cast<drflac_int16 *>(pcmData));
        if (samplesRead > 0) {
            /* Calculate frame duration in seconds */
            position += static_cast<float>(samplesRead) / static_cast<float>(sampleRate);
        }
        return samplesRead * channelCount;
    }

    void DecoderFLAC::setPosition(float pos)
    {
        if (!isInitialized) {
            LOG_ERROR("FLAC decoder not initialized");
            return;
        }
        drflac_seek_to_pcm_frame(flac, flac->totalPCMFrameCount * pos);

        // Calculate new position
        position = static_cast<float>(flac->totalPCMFrameCount) * pos / static_cast<float>(sampleRate);
    }

    /* Data encoded in UTF-8 */
    void DecoderFLAC::parseText(
        std::uint8_t *in, std::uint32_t taglen, std::uint32_t datalen, std::uint8_t *out, std::uint32_t outlen)
    {
        /* Little Endian here */
        const auto size = std::min(datalen - taglen, outlen - 1);
        memcpy(out, in + taglen, size);
        out[size] = '\0';
    }

    std::size_t DecoderFLAC::drflacRead(void *pUserData, void *pBufferOut, std::size_t bytesToRead)
    {
        const auto decoderContext = reinterpret_cast<DecoderFLAC *>(pUserData);
        return std::fread(pBufferOut, 1, bytesToRead, decoderContext->fd);
    }

    drflac_bool32 DecoderFLAC::drflacSeek(void *pUserData, int offset, drflac_seek_origin origin)
    {
        const auto decoderContext = reinterpret_cast<DecoderFLAC *>(pUserData);
        const auto seekError =
            std::fseek(decoderContext->fd, offset, origin == drflac_seek_origin_start ? SEEK_SET : SEEK_CUR);
        return (seekError == 0) ? DRFLAC_TRUE : DRFLAC_FALSE;
    }
} // namespace audio
