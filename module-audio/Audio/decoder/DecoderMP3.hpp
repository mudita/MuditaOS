// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Decoder.hpp"
#include <minimp3_ex.h>

namespace audio
{
    class DecoderMP3 : public Decoder
    {
      public:
        explicit DecoderMP3(const std::string &filePath);
        ~DecoderMP3();

        std::int32_t decode(std::uint32_t samplesToRead, std::int16_t *pcmData) override;

        void setPosition(float pos) override;

      private:
        std::unique_ptr<mp3dec_ex_t> dec;
        std::unique_ptr<mp3dec_io_t> io;

        // Callback for when data needs to be read from the client.
        //
        // pUserData   [in]  The user data that was assigned to mp3dec_io_t and family.
        // pBufferOut  [out] The output buffer.
        // bytesToRead [in]  The number of bytes to read.
        //
        // Returns the number of bytes actually read.
        //
        // A return value of less than bytesToRead indicates the end of the stream. Do _not_ return from this callback
        // until either the entire bytesToRead is filled or you have reached the end of the stream.
        static std::size_t mp3Read(void *pBufferOut, std::size_t bytesToRead, void *pUserData);

        // Callback for when data needs to be seeked.
        //
        // pUserData [in] The user data that was assigned to mp3dec_io_t and family.
        // offset    [in] The number of bytes to move, relative to the origin. Will never be negative.
        //
        // Returns whether the seek was successful.
        //
        // The offset will never be negative. It relates to the beginning position.
        static int mp3Seek(std::uint64_t offset, void *pUserData);
    };
} // namespace audio
