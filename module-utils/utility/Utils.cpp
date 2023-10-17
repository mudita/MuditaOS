// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Utils.hpp"
#include <crc32.h>
#include <filesystem>
#include <random>
#include <bsp/trng/trng.hpp>

namespace utils::filesystem
{
    namespace
    {
        constexpr auto crc_buf_len = 1024;
    } // namespace

    unsigned long computeFileCRC32(std::FILE *file) noexcept
    {
        auto buf = std::make_unique<unsigned char[]>(crc_buf_len);

        CRC32 digestCrc32;
        while (!std::feof(file)) {
            const auto dataLen = std::fread(buf.get(), 1, crc_buf_len, file);
            if (dataLen == 0) {
                break;
            }

            digestCrc32.add(buf.get(), dataLen);
        }

        return digestCrc32.getHashValue();
    }

    std::string getline(std::FILE *stream, uint32_t length) noexcept
    {
        const auto currentPosition = std::ftell(stream);

        // allocate memory to read number of signs defined by length param. Size of buffer is increased by 1 to add
        // string's null terminator.
        auto buffer = std::make_unique<char[]>(length + 1);

        const auto bytesRead = std::fread(buffer.get(), 1, length, stream);

        // search buffer for \n sign
        constexpr auto newLineSign = 0x0A;
        for (std::uint32_t i = 0; i < bytesRead; ++i) {
            if (buffer[i] == newLineSign) {
                buffer[i] = 0;
                std::fseek(stream, currentPosition + i + 1, SEEK_SET);
                break;
            }
        }

        return std::string(buffer.get());
    }
} // namespace utils::filesystem

namespace utils
{
    std::string generateRandomId(std::size_t length) noexcept
    {
        if (length == 0) {
            return "";
        }

        const std::string charset("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        const auto pseudoRandomSource =
            std::make_unique<std::mt19937>(bsp::trng::getRandomValue()); // Seed MT19937 with TRNG value
        std::uniform_int_distribution<> distribution(0, charset.size() - 1);

        std::string randomString(length, '\0');
        for (std::size_t i = 0; i < length; ++i) {
            randomString[i] = charset[distribution(*pseudoRandomSource)];
        }

        return randomString;
    }
} // namespace utils
