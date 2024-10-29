// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_NO_STDIO

#include "DecoderCommon.hpp"
#include "DecoderMP3.hpp"
#include <cstdio>

namespace audio
{
    DecoderMP3::DecoderMP3(const std::string &filePath)
        : Decoder(filePath), dec(std::make_unique<mp3dec_ex_t>()), io(std::make_unique<mp3dec_io_t>())
    {
        if (fileSize == 0) {
            return;
        }

        io->read      = mp3Read;
        io->read_data = this;
        io->seek      = mp3Seek;
        io->seek_data = this;
        dec->io       = io.get();

        const auto decoderStatus = mp3dec_ex_open_cb(dec.get(), dec->io, MP3D_SEEK_TO_SAMPLE | MP3D_DO_NOT_SCAN);
        if (decoderStatus != 0) {
            LOG_ERROR("Failed to open minimp3, error: %d", decoderStatus);
            return;
        }

        /* NOTE: Always convert to S16LE as an internal format */
        channelCount  = dec->info.channels;
        sampleRate    = dec->info.hz;
        bitsPerSample = 16;
        isInitialized = true;
    }

    DecoderMP3::~DecoderMP3()
    {
        if (!isInitialized) {
            return;
        }
        mp3dec_ex_close(dec.get());
        isInitialized = false;
    }

    void DecoderMP3::setPosition(float pos)
    {
        if (!isInitialized) {
            LOG_ERROR("MP3 decoder not initialized");
            return;
        }
        mp3dec_ex_seek(dec.get(), pos);
    }

    std::int32_t DecoderMP3::decode(std::uint32_t samplesToRead, std::int16_t *pcmData)
    {
        const auto samplesRead = mp3dec_ex_read(dec.get(), reinterpret_cast<mp3d_sample_t *>(pcmData), samplesToRead);
        if (samplesRead > 0) {
            /* Calculate frame duration in seconds */
            const auto samplesPerChannel = static_cast<float>(samplesRead) / static_cast<float>(channelCount);
            position += samplesPerChannel / static_cast<float>(sampleRate);
        }
        else if (!fileExists(fd)) {
            /* Unfortunately this second check of file existence is needed
             * to verify whether lack of new samples was caused by EOF or by
             * deletion of the file. */
            LOG_WARN("File '%s' was deleted during playback!", filePath.c_str());
            return fileDeletedRetCode;
        }
        return samplesRead;
    }

    std::size_t DecoderMP3::mp3Read(void *pBufferOut, std::size_t bytesToRead, void *pUserData)
    {
        const auto decoderContext = reinterpret_cast<DecoderMP3 *>(pUserData);

        /* Check if the file exists - std::fread happily returns bytesToRead if
         * requested to read from deleted file, what causes decoding library
         * to enter infinite loop of reading. */
        if (!fileExists(decoderContext->fd)) {
            return 0;
        }
        return std::fread(pBufferOut, 1, bytesToRead, decoderContext->fd);
    }

    int DecoderMP3::mp3Seek(std::uint64_t offset, void *pUserData)
    {
        const auto decoderContext = reinterpret_cast<DecoderMP3 *>(pUserData);
        return std::fseek(decoderContext->fd, offset, SEEK_SET);
    }
} // namespace audio
