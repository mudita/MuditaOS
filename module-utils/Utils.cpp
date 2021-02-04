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

    std::FILE *openFile(const std::filesystem::path &filePath) noexcept
    {
        return std::fopen(filePath.c_str(), "r");
    }

    std::string readFile(std::FILE *file) noexcept
    {
        uint32_t fsize = utils::filesystem::filelength(file);
        auto stream    = std::make_unique<char[]>(fsize + 1);
        std::fread(stream.get(), 1, fsize, file);

        return stream.get();
    }

    void closeFile(std::FILE *file) noexcept
    {
        if (file != nullptr) {
            std::fclose(file);
        }
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
