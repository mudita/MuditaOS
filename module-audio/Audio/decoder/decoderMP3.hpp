// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Decoder.hpp"
#include <minimp3.h>
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

        uint32_t decode(uint32_t samplesToRead, int16_t *pcmData) override;

        void setPosition(float pos) override;

      private:
        bool find_first_valid_frame();

        uint32_t get_frames_count();

        const uint32_t DECODER_BUFFER_SIZE = 1024 * 24;

        std::unique_ptr<mp3dec_t> mp3d;

        std::unique_ptr<uint8_t[]> decoderBuffer = nullptr;
        uint32_t decoderBufferIdx                = 0;

        std::unique_ptr<uint16_t[]> pcmsamplesbuffer = nullptr;
        uint32_t pcmsamplesbuffer_idx                = 0;

        const size_t pcmsamplesbuffer_size = (8192 + MINIMP3_MAX_SAMPLES_PER_FRAME) * sizeof(int16_t);

        uint32_t samplesPerFrame = 0;

        // Variables below are used during decoding procedure
        uint32_t firstValidFrameByteSize   = 0;
        uint32_t firstValidFrameFileOffset = 0;
        bool lastRefill                    = false;
        bool decoderNotFirstRun            = false;
        uint32_t bytesAvailable            = 0;
    };

} // namespace audio
