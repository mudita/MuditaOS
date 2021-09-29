// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/filesystem/FileContext.hpp>
#include <log/log.hpp>
#include <utility>

FileContext::FileContext(const std::filesystem::path &path,
                         std::size_t size,
                         std::size_t chunkSize,
                         const std::string &openMode,
                         std::size_t offset)
    : path(path), size(size), offset(offset), chunkSize(chunkSize)
{
    if (!size || !chunkSize) {
        throw std::invalid_argument("Invalid FileContext arguments");
    }

    file = std::fopen(path.c_str(), openMode.c_str());
    if (!file) {
        throw std::runtime_error("File open error");
    }

    constexpr size_t streamBufferSize = 64 * 1024;
    streamBuffer                      = std::make_unique<char[]>(streamBufferSize);
    setvbuf(file, streamBuffer.get(), _IOFBF, streamBufferSize);

    runningCrc32Digest.reset();
}

FileContext::~FileContext()
{
    std::fclose(file);
}

FileReadContext::FileReadContext(const std::filesystem::path &path,
                                 std::size_t size,
                                 std::size_t chunkSize,
                                 std::size_t offset)
    : FileContext(path, size, chunkSize, "rb", offset)
{}

FileReadContext::~FileReadContext()
{}

FileWriteContext::FileWriteContext(const std::filesystem::path &path,
                                   std::size_t size,
                                   std::size_t chunkSize,
                                   std::string crc32Digest,
                                   std::size_t offset)
    : FileContext(path, size, chunkSize, "wb", offset), crc32Digest(std::move(crc32Digest))
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

    std::fseek(file, offset, SEEK_SET);

    auto dataLeft = std::min(static_cast<std::size_t>(chunkSize), (size - offset));

    std::vector<std::uint8_t> buffer(dataLeft);

    auto dataRead = std::fread(buffer.data(), sizeof(int8_t), dataLeft, file);

    if (dataRead != dataLeft) {
        LOG_ERROR("File %s read error", path.c_str());
        throw std::runtime_error("File read error");
    }

    runningCrc32Digest.add(buffer.data(), dataRead);

    LOG_DEBUG("Read %u bytes", static_cast<unsigned int>(dataRead));
    advanceFileOffset(dataRead);

    if (reachedEOF()) {
        LOG_INFO("Reached EOF");
    }

    return buffer;
}

auto FileWriteContext::write(const std::vector<std::uint8_t> &data) -> void
{
    LOG_DEBUG("Sending file data");

    std::fseek(file, offset, SEEK_SET);

    auto dataLeft = std::min(static_cast<std::size_t>(chunkSize), (size - offset));

    auto dataWritten = std::fwrite(reinterpret_cast<const char *>(data.data()), sizeof(int8_t), dataLeft, file);

    if (dataWritten != dataLeft) {
        LOG_ERROR("File %s write error", path.c_str());
        throw std::runtime_error("File write error");
    }

    runningCrc32Digest.add(data.data(), dataWritten);

    LOG_DEBUG("Written %u bytes", static_cast<unsigned int>(dataWritten));

    advanceFileOffset(dataWritten);

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
    std::filesystem::remove(path);
}
