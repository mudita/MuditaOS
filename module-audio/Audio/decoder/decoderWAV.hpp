// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Decoder.hpp"
#include <vector>

namespace audio
{

    class decoderWAV : public Decoder
    {

      public:
        decoderWAV(const char *fileName);

        uint32_t decode(uint32_t samplesToRead, int16_t *pcmData) override;

        void setPosition(float pos) override;

      private:
        auto getBitWidth() -> unsigned int override;

        using WAVE_FormatTypeDef = struct
        {
            uint32_t ChunkID;       /* 0 */
            uint32_t FileSize;      /* 4 */
            uint32_t FileFormat;    /* 8 */
            uint32_t SubChunk1ID;   /* 12 */
            uint32_t SubChunk1Size; /* 16*/
            uint16_t AudioFormat;   /* 20 */
            uint16_t NbrChannels;   /* 22 */
            uint32_t SampleRate;    /* 24 */

            uint32_t ByteRate;      /* 28 */
            uint16_t BlockAlign;    /* 32 */
            uint16_t BitPerSample;  /* 34 */
            uint32_t SubChunk2ID;   /* 36 */
            uint32_t SubChunk2Size; /* 40 */
        };

        std::vector<int32_t> pcmsamplesbuffer;
        WAVE_FormatTypeDef waveHeader;
        uint32_t bitsPerSample;
    };

} // namespace audio
