// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Encoder.hpp"
#include "EncoderWAV.hpp"

#include <Utils.hpp>

#include <string>
#include <memory>
#include <cstring>
#include <filesystem>

namespace audio
{

    Encoder::Encoder(const std::string &filePath, const Format &frmt) : format(frmt), filePath(filePath)
    {
        fd = std::fopen(filePath.c_str(), "w");
        if (fd == nullptr) {
            return;
        }
        constexpr size_t streamBufferSize = 6;
        streamBuffer                      = std::make_unique<char[]>(streamBufferSize);
        setvbuf(fd, streamBuffer.get(), _IOFBF, streamBufferSize);
        isInitialized = true;
    }

    Encoder::~Encoder()
    {
        if (fd != nullptr) {
            std::fclose(fd);
        }
    }

    std::unique_ptr<Encoder> Encoder::Create(const std::string &filePath, const Format &frmt)
    {
        const auto extension          = std::filesystem::path(filePath).extension();
        const auto extensionLowercase = utils::stringToLowercase(extension);

        std::unique_ptr<Encoder> enc;
        if (extensionLowercase == ".wav") {
            enc = std::make_unique<EncoderWAV>(filePath, frmt);
        }
        else {
            return nullptr;
        }

        if (enc->isInitialized) {
            return enc;
        }

        return nullptr;
    }

} // namespace audio
