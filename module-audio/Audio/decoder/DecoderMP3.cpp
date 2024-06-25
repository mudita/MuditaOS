// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO

#include "DecoderCommon.hpp"
#include "DecoderMP3.hpp"
#include <cstdio>

namespace
{
    signed skipID3V2TagIfPresent(std::FILE *fd)
    {
        constexpr auto ID3V2FrameOffset            = 0;
        constexpr auto ID3V2FrameHeaderSize        = 10;
        constexpr auto ID3V2FrameMagicString       = "ID3";
        constexpr auto ID3V2FrameMagicStringLength = 3;
        std::uint8_t frameBuffer[ID3V2FrameHeaderSize];

        /* Seek to the beginning of the frame and read frame's header */
        if (std::fseek(fd, ID3V2FrameOffset, SEEK_SET) != 0) {
            return -EIO;
        }
        if (std::fread(frameBuffer, sizeof(*frameBuffer), ID3V2FrameHeaderSize, fd) != ID3V2FrameHeaderSize) {
            return -EIO;
        }

        /* Check magic */
        if (strncmp(reinterpret_cast<const char *>(frameBuffer), ID3V2FrameMagicString, ID3V2FrameMagicStringLength) !=
            0) {
            return 0;
        }

        /* The tag size (minus the 10-byte header) is encoded into four bytes,
         * but the most significant bit needs to be masked in each byte.
         * Those frame indices are just copied from the ID3V2 docs. */
        const auto ID3V2TagTotalSize = (((frameBuffer[6] & 0x7F) << 21) | ((frameBuffer[7] & 0x7F) << 14) |
                                        ((frameBuffer[8] & 0x7F) << 7) | ((frameBuffer[9] & 0x7F) << 0)) +
                                       ID3V2FrameHeaderSize;

        /* Skip the tag */
        if (std::fseek(fd, ID3V2FrameOffset + ID3V2TagTotalSize, SEEK_SET) != 0) {
            return -EIO;
        }
        return ID3V2TagTotalSize;
    }
} // namespace

namespace audio
{
    DecoderMP3::DecoderMP3(const std::string &filePath) : Decoder(filePath), mp3(std::make_unique<drmp3>())
    {
        if (fileSize == 0) {
            return;
        }

        const auto tagSkipStatus = skipID3V2TagIfPresent(fd);
        if (tagSkipStatus < 0) {
            LOG_ERROR("Failed to skip ID3V2 tag, error %d", tagSkipStatus);
        }
        else if (tagSkipStatus == 0) {
            LOG_INFO("No ID3V2 tag to skip");
        }

        if (drmp3_init(mp3.get(), drmp3Read, drmp3Seek, this, nullptr) == DRMP3_FALSE) {
            LOG_ERROR("Unable to initialize MP3 decoder");
            return;
        }

        /* NOTE: Always convert to S16LE as an internal format */
        channelCount  = mp3->channels;
        sampleRate    = mp3->sampleRate;
        bitsPerSample = 16;
        isInitialized = true;
    }

    DecoderMP3::~DecoderMP3()
    {
        if (!isInitialized) {
            return;
        }
        drmp3_uninit(mp3.get());
        isInitialized = false;
    }

    void DecoderMP3::setPosition(float pos)
    {
        if (!isInitialized) {
            LOG_ERROR("MP3 decoder not initialized");
            return;
        }
        const auto totalFramesCount = drmp3_get_pcm_frame_count(mp3.get());
        drmp3_seek_to_pcm_frame(mp3.get(), totalFramesCount * pos);
        position = static_cast<float>(totalFramesCount) * pos / static_cast<float>(sampleRate);
    }

    std::int32_t DecoderMP3::decode(std::uint32_t samplesToRead, std::int16_t *pcmData)
    {
        const auto samplesRead = drmp3_read_pcm_frames_s16(
            mp3.get(), samplesToRead / channelCount, reinterpret_cast<drmp3_int16 *>(pcmData));
        if (samplesRead > 0) {
            /* Calculate frame duration in seconds */
            position += static_cast<float>(samplesRead) / static_cast<float>(sampleRate);
        }
        else if (!fileExists(fd)) {
            /* Unfortunately this second check of file existence is needed
             * to verify whether lack of new samples was caused by EOF or by
             * deletion of the file. */
            LOG_WARN("File '%s' was deleted during playback!", filePath.c_str());
            return fileDeletedRetCode;
        }
        return samplesRead * channelCount;
    }

    std::size_t DecoderMP3::drmp3Read(void *pUserData, void *pBufferOut, std::size_t bytesToRead)
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

    drmp3_bool32 DecoderMP3::drmp3Seek(void *pUserData, int offset, drmp3_seek_origin origin)
    {
        const auto decoderContext = reinterpret_cast<DecoderMP3 *>(pUserData);
        const auto seekError =
            std::fseek(decoderContext->fd, offset, origin == drmp3_seek_origin_start ? SEEK_SET : SEEK_CUR);
        return (seekError == 0) ? DRMP3_TRUE : DRMP3_FALSE;
    }
} // namespace audio
