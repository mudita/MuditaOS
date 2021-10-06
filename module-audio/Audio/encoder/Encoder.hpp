// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <cstdio>
#include <string>

namespace audio
{

    class Encoder
    {
      public:
        struct Format
        {
            uint32_t chanNr;
            uint32_t sampleRate;
        };

        static std::unique_ptr<Encoder> Create(const char *file, const Format &frmt);

        Encoder(const char *file, const Format &frmt);

        virtual ~Encoder();

        virtual uint32_t Encode(uint32_t samplesToWrite, int16_t *pcmData) = 0;

        float getCurrentPosition()
        {
            return position;
        }

        uint32_t GetFileSize()
        {
            return fileSize;
        }

        const Format format;

      protected:
        float position = 0;
        std::FILE *fd  = nullptr;
        std::unique_ptr<char[]> streamBuffer;
        uint32_t fileSize = 0;
        std::string filePath;

        bool isInitialized = false;
    };

} // namespace audio
