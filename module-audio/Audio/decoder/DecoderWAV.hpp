// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Decoder.hpp"
#include <src/dr_wav.h>

namespace audio
{
    class DecoderWAV : public Decoder
    {
      public:
        explicit DecoderWAV(const std::string &filePath);
        ~DecoderWAV();

        std::int32_t decode(std::uint32_t samplesToRead, std::int16_t *pcmData) override;

        void setPosition(float pos) override;

      private:
        std::unique_ptr<drwav> wav;
    };
} // namespace audio
