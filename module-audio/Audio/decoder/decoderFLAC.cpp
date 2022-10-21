// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Utils.hpp>
#include "decoderFLAC.hpp"
#include "flac/flacfile.h"

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_OGG
#define DR_FLAC_NO_CRC
#define DR_FLAC_NO_SIMD
#include <src/dr_flac.h>

namespace audio
{

    decoderFLAC::decoderFLAC(const std::string &filePath) : Decoder(filePath)
    {
        if (fileSize == 0) {
            return;
        }

        flac = drflac_open(drflac_read, drflac_seek, this, nullptr);
        if (flac == nullptr) {
            LOG_ERROR("Unable to initialize FLAC decoder");
            return;
        }

        chanNumber = flac->channels;
        sampleRate = flac->sampleRate;
        // NOTE: Always convert to S16LE as internal format
        bitsPerSample = 16;
        isInitialized = true;
    }

    decoderFLAC::~decoderFLAC()
    {
        drflac_close(flac);
    }

    std::uint32_t decoderFLAC::decode(std::uint32_t samplesToRead, int16_t *pcmData)
    {
        std::uint32_t samples_read =
            drflac_read_pcm_frames_s16(flac, samplesToRead / chanNumber, reinterpret_cast<drflac_int16 *>(pcmData));
        if (samples_read > 0) {
            /* Calculate frame duration in seconds */
            position += static_cast<float>(samples_read) / static_cast<float>(sampleRate);
        }

        return samples_read * chanNumber;
    }

    void decoderFLAC::setPosition(float pos)
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
    void decoderFLAC::flac_parse_text(
        std::uint8_t *in, std::uint32_t taglen, std::uint32_t datalen, std::uint8_t *out, std::uint32_t outlen)
    {
        /* Little Endian here. */
        std::uint32_t size = ((datalen - taglen) > (outlen - 1)) ? (outlen - 1) : (datalen - taglen);
        memcpy(out, in + taglen, size);
        out[size] = '\0';
    }

    size_t decoderFLAC::drflac_read(void *pUserData, void *pBufferOut, size_t bytesToRead)
    {
        const auto decoderContext = reinterpret_cast<decoderFLAC *>(pUserData);
        return std::fread(pBufferOut, 1, bytesToRead, decoderContext->fd);
    }

    drflac_bool32 decoderFLAC::drflac_seek(void *pUserData, int offset, drflac_seek_origin origin)
    {
        const auto decoderContext = reinterpret_cast<decoderFLAC *>(pUserData);
        const int seekError =
            std::fseek(decoderContext->fd, offset, origin == drflac_seek_origin_start ? SEEK_SET : SEEK_CUR);
        return (seekError == 0) ? DRFLAC_TRUE : DRFLAC_FALSE;
    }

} // namespace audio
