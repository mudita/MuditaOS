// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Encoder.hpp"

namespace audio
{

    class EncoderWAV : public Encoder
    {
      public:
        EncoderWAV(const std::string &filePath, const Encoder::Format &frmt);

        ~EncoderWAV();

        std::uint32_t Encode(std::uint32_t samplesToWrite, std::int16_t *pcmData) override final;

      private:
        using WAVE_FormatTypeDef = struct
        {
            std::uint32_t ChunkID;       /* 0 */
            std::uint32_t FileSize;      /* 4 */
            std::uint32_t FileFormat;    /* 8 */
            std::uint32_t SubChunk1ID;   /* 12 */
            std::uint32_t SubChunk1Size; /* 16*/
            std::uint16_t AudioFormat;   /* 20 */
            std::uint16_t NbrChannels;   /* 22 */
            std::uint32_t SampleRate;    /* 24 */

            std::uint32_t ByteRate;      /* 28 */
            std::uint16_t BlockAlign;    /* 32 */
            std::uint16_t BitPerSample;  /* 34 */
            std::uint32_t SubChunk2ID;   /* 36 */
            std::uint32_t SubChunk2Size; /* 40 */
        };

        void HeaderInit(const WAVE_FormatTypeDef &pWaveFormatStruct);

        void HeaderUpdate();

        std::uint8_t pHeaderBuff[44] = {0};
    };

} // namespace audio
