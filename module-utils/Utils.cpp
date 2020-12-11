// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Utils.hpp"

namespace utils::filesystem
{
    long int filelength(std::FILE *file) noexcept
    {
        if (file == nullptr) {
            return 0;
        }
        std::fseek(file, 0, SEEK_END);
        return std::ftell(file);
    }

    void computeCRC32(std::FILE *file, unsigned long *outCrc32) noexcept
    {
        if (outCrc32 == nullptr)
            return;

        auto buf = std::make_unique<unsigned char[]>(purefs::buffer::crc_buf);
        size_t bufLen;

        *outCrc32 = 0;

        while (!std::feof(file)) {
            bufLen = std::fread(buf.get(), 1, purefs::buffer::crc_buf, file);
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
} // namespace utils::filesystem
