// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
            std::uint32_t chanNr;
            std::uint32_t sampleRate;
        };

        static std::unique_ptr<Encoder> Create(const std::string &filePath, const Format &frmt);

        Encoder(const std::string &filePath, const Format &frmt);

        virtual ~Encoder();

        virtual std::uint32_t Encode(std::uint32_t samplesToWrite, std::int16_t *pcmData) = 0;

        float getCurrentPosition()
        {
            return position;
        }

        std::uint32_t GetFileSize()
        {
            return fileSize;
        }

        const Format format;

      protected:
        float position = 0;
        std::FILE *fd  = nullptr;
        std::unique_ptr<char[]> streamBuffer;
        std::uint32_t fileSize = 0;
        std::string filePath;

        bool isInitialized = false;
    };

} // namespace audio
