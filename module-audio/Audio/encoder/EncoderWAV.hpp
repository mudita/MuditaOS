// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Encoder.hpp"

namespace audio
{

    class EncoderWAV : public Encoder
    {
      public:
        EncoderWAV(const char *fileName, const Encoder::Format &frmt);

        ~EncoderWAV();

        uint32_t Encode(uint32_t samplesToWrite, int16_t *pcmData) override final;

      private:
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

        void HeaderInit(const WAVE_FormatTypeDef &pWaveFormatStruct);

        void HeaderUpdate();

        uint8_t pHeaderBuff[44] = {0};
    };

} // namespace audio
