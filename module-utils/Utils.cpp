// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Utils.hpp"
#include <purefs/fs/filesystem.hpp>
#include <crc32/crc32.h>

namespace utils::filesystem
{
    namespace
    {
        inline constexpr auto crc_buf = 1024;
    } // namespace

    long int filelength(std::FILE *file) noexcept
    {
        if (file == nullptr) {
            return 0;
        }
        const auto startPosition = std::ftell(file);
        std::fseek(file, 0, SEEK_END);
        const auto endPosition = std::ftell(file);
        std::fseek(file, startPosition, SEEK_SET);
        return endPosition;
    }

    void computeCRC32(std::FILE *file, unsigned long *outCrc32) noexcept
    {
        if (outCrc32 == nullptr)
            return;

        auto buf = std::make_unique<unsigned char[]>(crc_buf);
        size_t bufLen;

        *outCrc32 = 0;

        while (!std::feof(file)) {
            bufLen = std::fread(buf.get(), 1, crc_buf, file);
            if (bufLen <= 0)
                break;

            *outCrc32 = Crc32_ComputeBuf(*outCrc32, buf.get(), bufLen);
        }
    }

    std::string generateRandomId(std::size_t length) noexcept
    {
        const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        std::random_device random_device;
        std::mt19937 generator(random_device());
        generator.seed(utils::time::TimeConversion::toTime_t(utils::time::TimePointNow()));
        std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

        std::string random_string;

        for (std::size_t i = 0; i < length; ++i) {
            random_string += CHARACTERS[distribution(generator)];
        }

        return random_string;
    }

    std::string getline(std::FILE *stream, uint32_t length) noexcept
    {
        std::uint32_t currentPosition = std::ftell(stream);

        // allocate memory to read number of signs defined by length param. Size of buffer is increased by 1 to add
        // string's null terminator.
        auto buffer = std::make_unique<char[]>(length + 1);

        std::uint32_t bytesRead = std::fread(buffer.get(), 1, length, stream);

        // search buffer for /n sign
        constexpr auto newLineSign = 0x0A;
        for (std::uint32_t i = 0; i < bytesRead; ++i) {
            if (buffer[i] == newLineSign) {
                buffer[i] = 0;
                std::fseek(stream, currentPosition + i + 1, SEEK_SET);
                break;
            }
        }

        std::string ret = std::string(buffer.get());

        return ret;
    }
} // namespace utils::filesystem
