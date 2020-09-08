/*
 *  @file decoderMP3.h
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 04.04.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef UNTITLED1_DECODERMP3_HPP
#define UNTITLED1_DECODERMP3_HPP

#include "minimp3/minimp3.h"

#include <cstring>
#include "decoder.hpp"

extern "C"
{
#include "xing_header.h"
};

namespace audio
{

    class decoderMP3 : public decoder
    {

      public:
        decoderMP3(const char *fileName);

        ~decoderMP3() = default;

        uint32_t decode(uint32_t samplesToRead, int16_t *pcmData) override;

        void setPosition(float pos) override;

      private:
        void fetchTagsSpecific() override;

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

#endif // UNTITLED1_DECODERMP3_HPP
