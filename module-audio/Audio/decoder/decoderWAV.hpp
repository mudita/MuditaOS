/*
 *  @file decoderWAV.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 06.04.19
 *  @brief 
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef UNTITLED1_DECODERWAV_HPP
#define UNTITLED1_DECODERWAV_HPP

#include "decoder.h"

namespace decoder {


    class decoderWAV : public decoder {

    public:

        decoderWAV(const char *fileName);

        ~decoderWAV() {}

        std::unique_ptr<Tags> fetchTags() override;

        uint32_t decode(uint32_t samplesToRead, int16_t *pcmData) override;

        void setPosition(float pos) override;

    private:

        using WAVE_FormatTypeDef = struct {
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

        WAVE_FormatTypeDef waveHeader;
        uint32_t bitsPerSample;
        std::unique_ptr<Tags> tag = nullptr;

    };

}


#endif //UNTITLED1_DECODERWAV_HPP
