// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Utils.hpp"
#include <filesystem>
#include <crc32.h>

namespace utils::filesystem
{
    namespace
    {
        inline constexpr auto crc_buf_len = 1024;
    } // namespace

    unsigned long computeFileCRC32(std::FILE *file) noexcept
    {
        auto buf = std::make_unique<unsigned char[]>(crc_buf_len);

        unsigned long crc32 = 0;
        while (!std::feof(file)) {
            size_t dataLen = std::fread(buf.get(), 1, crc_buf_len, file);
            if (dataLen == 0) {
                return crc32;
            }

            crc32 = Crc32_ComputeBuf(crc32, buf.get(), dataLen);
        }

        return crc32;
    }

    std::string generateRandomId(std::size_t length) noexcept
    {
        const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        std::random_device random_device;
        std::mt19937 generator(random_device());
        generator.seed(utils::time::Timestamp().getTime());
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

namespace utils
{
    std::vector<std::uint8_t> hexToBytes(const std::string &hex)
    {
        std::vector<std::uint8_t> bytes;

        for (unsigned int i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            std::uint8_t byte      = std::stoull(byteString.c_str(), nullptr, 16);
            bytes.push_back(byte);
        }
        return bytes;
    }
    std::string bytesToHex(const std::vector<std::uint8_t> &bytes)
    {
        std::stringstream s;
        s.fill('0');
        for (auto const &b : bytes)
            s << std::setw(2) << std::hex << (unsigned short)b;
        return s.str();
    }

} // namespace utils
