// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Decoder.hpp"
#include <src/dr_wav.h>

namespace audio
{
    class DecoderWAV : public Decoder
    {
      public:
        explicit DecoderWAV(const std::string &filePath);
        ~DecoderWAV();

        std::int32_t decode(std::uint32_t samplesToRead, std::int16_t *pcmData) override;

        void setPosition(float pos) override;

      private:
        std::unique_ptr<drwav> wav;

        // Callback for when data needs to be read from the client.
        //
        // pUserData   [in]  The user data that was passed to drwav_open() and family.
        // pBufferOut  [out] The output buffer.
        // bytesToRead [in]  The number of bytes to read.
        //
        // Returns the number of bytes actually read.
        //
        // A return value of less than bytesToRead indicates the end of the stream. Do _not_ return from this callback
        // until either the entire bytesToRead is filled or you have reached the end of the stream.
        static std::size_t drwavRead(void *pUserData, void *pBufferOut, std::size_t bytesToRead);

        // Callback for when data needs to be seeked.
        //
        // pUserData [in] The user data that was passed to drwav_init() and family.
        // offset    [in] The number of bytes to move, relative to the origin. Will never be negative.
        // origin    [in] The origin of the seek - the current position or the start of the stream.
        //
        // Returns whether the seek was successful.
        //
        // The offset will never be negative. Whether it is relative to the beginning or current position is
        // determined by the "origin" parameter which will be either drwav_seek_origin_start or
        // drwav_seek_origin_current.
        static drwav_bool32 drwavSeek(void *pUserData, int offset, drwav_seek_origin origin);
    };
} // namespace audio
