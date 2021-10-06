// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Encoder.hpp"
#include "EncoderWAV.hpp"

#include <stdint.h>
#include <string>
#include <memory>
#include <cstring>

namespace audio
{

    Encoder::Encoder(const char *fileName, const Format &frmt) : format(frmt), filePath(fileName)
    {

        fd = std::fopen(fileName, "w");
        if (fd == NULL) {
            return;
        }
        constexpr size_t streamBufferSize = 6;
        streamBuffer                      = std::make_unique<char[]>(streamBufferSize);
        setvbuf(fd, streamBuffer.get(), _IOFBF, streamBufferSize);
        isInitialized = true;
    }

    Encoder::~Encoder()
    {
        if (fd) {
            std::fclose(fd);
        }
    }

    std::unique_ptr<Encoder> Encoder::Create(const char *file, const Format &frmt)
    {
        std::unique_ptr<Encoder> enc;
        if ((strstr(file, ".wav") != NULL) || (strstr(file, ".WAV") != NULL)) {
            enc = std::make_unique<EncoderWAV>(file, frmt);
        }
        else {
            return nullptr;
        }

        if (enc->isInitialized) {
            return enc;
        }
        else {
            return nullptr;
        }
    }

} // namespace audio
