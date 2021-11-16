// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/filesystem/FileContext.hpp>
#include <log/log.hpp>
#include <utility>
#include <fstream>

FileContext::FileContext(const std::filesystem::path &path,
                         std::size_t size,
                         std::size_t chunkSize,
                         std::size_t offset)
    : path(path), size(size), offset(offset), chunkSize(chunkSize)
{
    if (!size || !chunkSize) {
        throw std::invalid_argument("Invalid FileContext arguments");
    }

    runningCrc32Digest.reset();
}

FileContext::~FileContext()
{
}

FileReadContext::FileReadContext(const std::filesystem::path &path,
                                 std::size_t size,
                                 std::size_t chunkSize,
                                 std::size_t offset)
    : FileContext(path, size, chunkSize, offset)
{}

FileReadContext::~FileReadContext()
{}

FileWriteContext::FileWriteContext(const std::filesystem::path &path,
                                   std::size_t size,
                                   std::size_t chunkSize,
                                   std::string crc32Digest,
                                   std::size_t offset)
    : FileContext(path, size, chunkSize, offset), crc32Digest(std::move(crc32Digest))
{}

FileWriteContext::~FileWriteContext()
{}

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

auto FileContext::fileHash() const -> std::string
{
    return runningCrc32Digest.getHash();
}

auto FileReadContext::read() -> std::vector<std::uint8_t>
{
    LOG_DEBUG("Getting file data");

    std::ifstream file(path, std::ios::binary);

    if (!file.is_open() || file.fail()) {
        LOG_ERROR("File %s open error", path.c_str());
        throw std::runtime_error("File open error");
    }

    file.seekg(offset);

    auto dataLeft = std::min(static_cast<std::size_t>(chunkSize), (size - offset));

    std::vector<std::uint8_t> buffer(dataLeft);

    file.read(reinterpret_cast<char *>(buffer.data()), dataLeft);

    if (file.bad()) {
        LOG_ERROR("File %s read error", path.c_str());
        throw std::runtime_error("File read error");
    }

    runningCrc32Digest.add(buffer.data(), dataLeft);

    LOG_DEBUG("Read %u bytes", static_cast<unsigned int>(dataLeft));
    advanceFileOffset(dataLeft);

    if (reachedEOF()) {
        LOG_INFO("Reached EOF");
    }

    return buffer;
}

auto FileWriteContext::write(const std::vector<std::uint8_t> &data) -> void
{
    LOG_DEBUG("Sending file data");

    std::ofstream file(path, std::ios::binary | std::ios::app);

    if (!file.is_open() || file.fail()) {
        LOG_ERROR("File %s open error", path.c_str());
        throw std::runtime_error("File open error");
    }

    file.seekp(offset);

    auto dataLeft = std::min(static_cast<std::size_t>(chunkSize), (size - offset));

    file.write(reinterpret_cast<const char *>(data.data()), dataLeft);
    file.flush();

    if (file.bad()) {
        LOG_ERROR("File %s write error", path.c_str());
        throw std::runtime_error("File write error");
    }

    runningCrc32Digest.add(data.data(), dataLeft);

    LOG_DEBUG("Written %u bytes", static_cast<unsigned int>(dataLeft));

    advanceFileOffset(dataLeft);

    if (reachedEOF()) {
        LOG_INFO("Reached EOF of %s", path.c_str());
    }
}

auto FileWriteContext::crc32Matches() const -> bool
{
    LOG_DEBUG("Hash: %s", fileHash().c_str());
    return crc32Digest == fileHash();
}

auto FileWriteContext::removeFile() -> void
{
    std::error_code ec;
    std::filesystem::remove(path, ec);
}
