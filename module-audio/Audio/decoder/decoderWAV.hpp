// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Decoder.hpp"
#include <vector>

namespace audio
{
    namespace internal
    {
        struct wavContext;
    }
    class decoderWAV : public Decoder
    {

      public:
        explicit decoderWAV(const char *fileName);
        virtual ~decoderWAV();

        uint32_t decode(uint32_t samplesToRead, int16_t *pcmData) override;

        void setPosition(float pos) override;

      private:
        auto getBitWidth() -> unsigned int override;


        std::vector<int32_t> pcmsamplesbuffer;
        std::unique_ptr<internal::wavContext> decoderContext;
        uint32_t bitsPerSample;
    };

} // namespace audio
