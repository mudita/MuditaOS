// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO

#include "decoderCommon.hpp"
#include "decoderMP3.hpp"
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
    decoderMP3::decoderMP3(const std::string &filePath) : Decoder(filePath)
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
        const auto totalFramesCount = drmp3_get_pcm_frame_count(mp3.get());
        drmp3_seek_to_pcm_frame(mp3.get(), totalFramesCount * pos);
        position = static_cast<float>(totalFramesCount) * pos / static_cast<float>(sampleRate);
    }

    std::uint32_t decoderMP3::decode(std::uint32_t samplesToRead, std::int16_t *pcmData)
    {
        const auto samplesRead =
            drmp3_read_pcm_frames_s16(mp3.get(), samplesToRead / chanNumber, reinterpret_cast<drmp3_int16 *>(pcmData));
        if (samplesRead > 0) {
            position += static_cast<float>(samplesRead) / static_cast<float>(sampleRate);
        }

        return samplesRead * chanNumber;
    }

    std::size_t decoderMP3::drmp3_read(void *pUserData, void *pBufferOut, std::size_t bytesToRead)
    {
        const auto decoderContext = reinterpret_cast<decoderMP3 *>(pUserData);
        return !statFd(decoderContext->fd, "MP3 audio file deleted by user!")
                   ? 0
                   : std::fread(pBufferOut, 1, bytesToRead, decoderContext->fd);
    }

    drmp3_bool32 decoderMP3::drmp3_seek(void *pUserData, int offset, drmp3_seek_origin origin)
    {
        const auto decoderContext = reinterpret_cast<decoderMP3 *>(pUserData);
        const auto seekError =
            std::fseek(decoderContext->fd, offset, origin == drmp3_seek_origin_start ? SEEK_SET : SEEK_CUR);
        return (seekError == 0) ? DRMP3_TRUE : DRMP3_FALSE;
    }
} // namespace audio
