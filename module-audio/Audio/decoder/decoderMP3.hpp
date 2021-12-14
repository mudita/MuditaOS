// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Decoder.hpp"
#include <src/dr_mp3.h>
#include <cstring>

extern "C"
{
#include "xing_header.h"
};

namespace audio
{

    class decoderMP3 : public Decoder
    {

      public:
        decoderMP3(const char *fileName);

        ~decoderMP3();

        uint32_t decode(uint32_t samplesToRead, int16_t *pcmData) override;

        void setPosition(float pos) override;

      private:
        bool find_first_valid_frame();

        uint32_t get_frames_count();

        const uint32_t DECODER_BUFFER_SIZE = 1024 * 24;

        std::unique_ptr<drmp3> mp3;

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
        //        std::unique_ptr<uint8_t[]> decoderBuffer = nullptr;
        //        uint32_t decoderBufferIdx                = 0;
        //
        //        std::unique_ptr<uint16_t[]> pcmsamplesbuffer = nullptr;
        //        uint32_t pcmsamplesbuffer_idx                = 0;
        //
        //        const size_t pcmsamplesbuffer_size = (8192 + MINIMP3_MAX_SAMPLES_PER_FRAME) * sizeof(int16_t);
        //
        //        uint32_t samplesPerFrame = 0;
        //
        //        // Variables below are used during decoding procedure
        //        uint32_t firstValidFrameByteSize   = 0;
        //        uint32_t firstValidFrameFileOffset = 0;
        //        bool lastRefill                    = false;
        //        bool decoderNotFirstRun            = false;
        //        uint32_t bytesAvailable            = 0;
    };

} // namespace audio
