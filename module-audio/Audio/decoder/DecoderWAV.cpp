// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "DecoderWAV.hpp"
#include "DecoderCommon.hpp"
#include <log/log.hpp>
#include <memory>

#define DR_WAV_IMPLEMENTATION
#define DR_MP3_NO_STDIO

#include <src/dr_wav.h>

namespace audio
{
    DecoderWAV::DecoderWAV(const std::string &filePath) : Decoder(filePath), wav(std::make_unique<drwav>())
    {
        if (fileSize == 0) {
            return;
        }

        if (drwav_init(wav.get(), drwavRead, drwavSeek, this, nullptr) == DRWAV_FALSE) {
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

    auto DecoderWAV::setPosition(float pos) -> void
    {
        if (!isInitialized) {
            LOG_ERROR("WAV decoder not initialized");
            return;
        }

        const auto frameToSeek = static_cast<std::uint64_t>(wav->totalPCMFrameCount * pos);
        const auto status      = drwav_seek_to_pcm_frame(wav.get(), frameToSeek);
        if (status == DRWAV_FALSE) {
            LOG_ERROR("Failed to seek to frame %" PRIu64 " in file '%s'!", frameToSeek, filePath.c_str());
        }

        /* Calculate new position */
        position = frameToSeek / static_cast<float>(sampleRate);
    }

    auto DecoderWAV::rewind() -> void
    {
        setPosition(0.0f);
    }

    auto DecoderWAV::decode(std::uint32_t samplesToRead, std::int16_t *pcmData) -> std::int32_t
    {
        if (!isInitialized) {
            LOG_ERROR("WAV decoder not initialized");
            return 0;
        }

        const auto framesRead = drwav_read_pcm_frames_s16(wav.get(), samplesToRead / channelCount, pcmData);
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

    auto DecoderWAV::drwavRead(void *pUserData, void *pBufferOut, std::size_t bytesToRead) -> std::size_t
    {
        const auto decoderContext = static_cast<DecoderWAV *>(pUserData);

        /* Check if the file exists - std::fread happily returns bytesToRead if
         * requested to read from deleted file, what causes decoding library
         * to enter infinite loop of reading. */
        if (!fileExists(decoderContext->fd)) {
            return 0;
        }
        return std::fread(pBufferOut, 1, bytesToRead, decoderContext->fd);
    }

    auto DecoderWAV::drwavSeek(void *pUserData, int offset, drwav_seek_origin origin) -> drwav_bool32
    {
        const auto decoderContext = static_cast<DecoderWAV *>(pUserData);
        const auto seekError =
            std::fseek(decoderContext->fd, offset, (origin == drwav_seek_origin_start) ? SEEK_SET : SEEK_CUR);
        return (seekError == 0) ? DRWAV_TRUE : DRWAV_FALSE;
    }
} // namespace audio
