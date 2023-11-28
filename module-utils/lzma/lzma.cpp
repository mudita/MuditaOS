// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "lzma.hpp"

#include <fstream>
#include <minlzma.h>

namespace utils::lzma
{
    void decompress(const std::filesystem::path &src, const std::filesystem::path &dst)
    {
        std::ifstream srcFile{src, std::ios::binary};
        if (!srcFile.is_open()) {
            throw std::filesystem::filesystem_error("failed to open source file " + src.string(),
                                                    std::make_error_code(std::errc::no_such_file_or_directory));
        }

        const auto srcFileSize = std::filesystem::file_size(src);
        const auto inputBuffer = std::make_unique<std::uint8_t[]>(srcFileSize);

        srcFile.read(reinterpret_cast<char *>(inputBuffer.get()), static_cast<std::streamsize>(srcFileSize));

        /* Get decompressed file size */
        std::uint32_t decompressedDataSize = 0;
        if (!XzDecode(inputBuffer.get(), srcFileSize, nullptr, &decompressedDataSize)) {
            throw std::runtime_error("failed to obtain decompressed file size");
        }

        const auto outputBuffer = std::make_unique<std::uint8_t[]>(
            decompressedDataSize); // WHOLE LOTTA heap is allocated here unfortunately...
        if (!XzDecode(inputBuffer.get(), srcFileSize, outputBuffer.get(), &decompressedDataSize)) {
            throw std::runtime_error("failed to decompress " + src.string());
        }

        std::ofstream dstFile{dst, std::ios::binary};
        if (!dstFile.is_open()) {
            throw std::filesystem::filesystem_error("failed to open destination file " + dst.string(),
                                                    std::make_error_code(std::errc::no_such_file_or_directory));
        }
        dstFile.write(reinterpret_cast<const char *>(outputBuffer.get()),
                      static_cast<std::streamsize>(decompressedDataSize));
    }
} // namespace utils::lzma
