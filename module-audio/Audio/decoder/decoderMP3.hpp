// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Decoder.hpp"
#include <src/dr_mp3.h>

extern "C"
{
#include "xing_header.h"
};

namespace audio
{

    class decoderMP3 : public Decoder
    {

      public:
        explicit decoderMP3(const std::string &filePath);

        ~decoderMP3();

        std::uint32_t decode(std::uint32_t samplesToRead, std::int16_t *pcmData) override;

        void setPosition(float pos) override;

      private:
        std::unique_ptr<drmp3> mp3 = nullptr;

        // Callback for when data needs to be read from the client.
        //
        // pUserData   [in]  The user data that was passed to drflac_open() and family.
        // pBufferOut  [out] The output buffer.
        // bytesToRead [in]  The number of bytes to read.
        //
        // Returns the number of bytes actually read.
        //
        // A return value of less than bytesToRead indicates the end of the stream. Do _not_ return from this callback
        // until either the entire bytesToRead is filled or you have reached the end of the stream.
        static size_t drmp3_read(void *pUserData, void *pBufferOut, size_t bytesToRead);

        // Callback for when data needs to be seeked.
        //
        // pUserData [in] The user data that was passed to drflac_open() and family.
        // offset    [in] The number of bytes to move, relative to the origin. Will never be negative.
        // origin    [in] The origin of the seek - the current position or the start of the stream.
        //
        // Returns whether or not the seek was successful.
        //
        // The offset will never be negative. Whether or not it is relative to the beginning or current position is
        // determined by the "origin" parameter which will be either drflac_seek_origin_start or
        // drflac_seek_origin_current.
        static drmp3_bool32 drmp3_seek(void *pUserData, int offset, drmp3_seek_origin origin);
    };

} // namespace audio
