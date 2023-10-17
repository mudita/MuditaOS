// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Decoder.hpp"

namespace audio
{
    namespace internal
    {
        struct wavContext;
    }
    class decoderWAV : public Decoder
    {
      public:
        explicit decoderWAV(const std::string &filePath);
        virtual ~decoderWAV();

        std::uint32_t decode(std::uint32_t samplesToRead, std::int16_t *pcmData) override;

        void setPosition(float pos) override;

      private:
        std::unique_ptr<internal::wavContext> decoderContext;
    };

} // namespace audio
