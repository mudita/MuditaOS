// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FileContext.hpp"
#include <log.hpp>

FileContext::FileContext(std::filesystem::path path, std::size_t size, std::size_t offset, std::size_t chunkSize)
    : path(path), size(size), offset(offset), chunkSize(chunkSize)
{
    if (!size || !chunkSize) {
        throw std::invalid_argument("Invalid FileContext arguments");
    }
};

auto FileContext::advanceFileOffset(std::size_t bySize) -> void
{
    offset += bySize;
}

auto FileContext::reachedEOF() const -> bool
{
    return offset >= size;
}

auto FileContext::chunksInQuantity(std::size_t quantity) const -> std::size_t
{
    return (quantity + chunkSize - 1) / chunkSize;
}

auto FileContext::totalChunksInFile() const -> std::size_t
{
    return chunksInQuantity(size);
}

auto FileContext::expectedChunkInFile() const -> std::size_t
{
    return 1 + chunksInQuantity(offset);
}

auto FileContext::validateChunkRequest(std::uint32_t chunkNo) const -> bool
{
    return !(chunkNo < 1 || chunkNo > totalChunksInFile() || chunkNo != expectedChunkInFile());
}

auto FileContext::getDataForFile() -> std::vector<std::uint8_t>
{
    LOG_DEBUG("Getting file data");

    auto fileCloser = [](std::FILE *stream) { std::fclose(stream); };

    std::unique_ptr<std::FILE, decltype(fileCloser)> file(std::fopen(path.c_str(), "rb"), fileCloser);

    if (!file) {
        LOG_ERROR("File %s open error", path.c_str());
        return {};
    }

    std::fseek(file.get(), offset, SEEK_SET);

    auto dataLeft = std::min(static_cast<std::size_t>(chunkSize), (size - offset));

    std::vector<std::uint8_t> buffer(dataLeft);

    auto dataRead = std::fread(buffer.data(), sizeof(uint8_t), dataLeft, file.get());

    LOG_DEBUG("Read %u bytes", static_cast<unsigned int>(dataRead));

    if (dataRead != dataLeft) {
        LOG_ERROR("File %s read error", path.c_str());
        return {};
    }

    advanceFileOffset(buffer.size());

    if (reachedEOF()) {
        LOG_INFO("Reached EOF of %s", path.c_str());
    }

    return buffer;
}
