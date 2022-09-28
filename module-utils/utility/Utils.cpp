// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Utils.hpp"

#include <crc32.h>

#include <ctime>
#include <filesystem>

namespace utils::filesystem
{
    namespace
    {
        inline constexpr auto crc_buf_len = 1024;
    } // namespace

    unsigned long computeFileCRC32(std::FILE *file) noexcept
    {
        auto buf = std::make_unique<unsigned char[]>(crc_buf_len);

        CRC32 digestCrc32;
        while (!std::feof(file)) {
            size_t dataLen = std::fread(buf.get(), 1, crc_buf_len, file);
            if (dataLen == 0) {
                break;
            }

            digestCrc32.add(buf.get(), dataLen);
        }

        return digestCrc32.getHashValue();
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

namespace utils
{
    std::string generateRandomId(std::size_t length) noexcept
    {
        if (!length)
            return {};

        const std::string CHARACTERS("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        auto random_device = std::make_unique<std::random_device>();
        auto generator     = std::make_unique<std::mt19937>((*random_device)());
        generator->seed(std::time(nullptr));
        std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

        std::string random_string(length, '\0');

        for (std::size_t i = 0; i < length; ++i) {
            random_string[i] = CHARACTERS[distribution(*generator)];
        }

        return random_string;
    }

} // namespace utils
