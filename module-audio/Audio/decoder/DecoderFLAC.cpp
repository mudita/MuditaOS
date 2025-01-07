// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "DecoderFLAC.hpp"
#include "DecoderCommon.hpp"

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

    auto DecoderFLAC::decode(std::uint32_t samplesToRead, std::int16_t *pcmData) -> std::int32_t
    {
        const auto framesRead =
            drflac_read_pcm_frames_s16(flac, samplesToRead / channelCount, reinterpret_cast<drflac_int16 *>(pcmData));
        if (framesRead > 0) {
            /* Calculate frame duration in seconds */
            position += static_cast<float>(framesRead) / static_cast<float>(sampleRate);
        }
        else if (!fileExists(fd)) {
            /* Unfortunately this second check of file existence is needed
             * to verify whether lack of new samples was caused by EOF or by
             * deletion of the file. */
            LOG_WARN("File '%s' was deleted during playback!", filePath.c_str());
            return fileDeletedRetCode;
        }
        return framesRead * channelCount;
    }

    auto DecoderFLAC::setPosition(float pos) -> void
    {
        if (!isInitialized) {
            LOG_ERROR("FLAC decoder not initialized");
            return;
        }

        const auto frameToSeek = static_cast<std::uint64_t>(flac->totalPCMFrameCount * pos);
        const auto status      = drflac_seek_to_pcm_frame(flac, frameToSeek);
        if (status == DRFLAC_FALSE) {
            LOG_ERROR("Failed to seek to frame %" PRIu64 " in file '%s'!", frameToSeek, filePath.c_str());
        }

        // Calculate new position
        position = frameToSeek / static_cast<float>(sampleRate);
    }

    auto DecoderFLAC::rewind() -> void
    {
        setPosition(0.0f);
    }

    auto DecoderFLAC::drflacRead(void *pUserData, void *pBufferOut, std::size_t bytesToRead) -> std::size_t
    {
        const auto decoderContext = static_cast<DecoderFLAC *>(pUserData);

        /* Check if the file exists - std::fread happily returns bytesToRead if
         * requested to read from deleted file, what causes decoding library
         * to enter infinite loop of reading. */
        if (!fileExists(decoderContext->fd)) {
            return 0;
        }
        return std::fread(pBufferOut, 1, bytesToRead, decoderContext->fd);
    }

    auto DecoderFLAC::drflacSeek(void *pUserData, int offset, drflac_seek_origin origin) -> drflac_bool32
    {
        const auto decoderContext = static_cast<DecoderFLAC *>(pUserData);
        const auto seekError =
            std::fseek(decoderContext->fd, offset, (origin == drflac_seek_origin_start) ? SEEK_SET : SEEK_CUR);
        return (seekError == 0) ? DRFLAC_TRUE : DRFLAC_FALSE;
    }
} // namespace audio
