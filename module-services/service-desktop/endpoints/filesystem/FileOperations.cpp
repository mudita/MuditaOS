// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FileOperations.hpp"
#include <base64.h>
#include <log.hpp>

FileOperations &FileOperations::instance()
{
    static FileOperations instance;
    return instance;
}

auto FileOperations::createReceiveIDForFile(const std::filesystem::path &file) -> std::pair<std::uint32_t, std::size_t>
{
    auto myRxID = ++runningRxId;
    auto size   = std::filesystem::file_size(file);

    LOG_DEBUG("Creating myRxID %u", static_cast<unsigned>(myRxID));

    createFileContextFor(file, size, myRxID);

    return std::make_pair(myRxID, size);
}

auto FileOperations::createFileContextFor(const std::filesystem::path &file, std::size_t fileSize, std::uint32_t rxId)
    -> void
{
    transfers.insert(std::make_pair(rxId, std::make_unique<FileContext>(file, fileSize, 0)));
}

auto FileOperations::getDataForFileCtx(const FileContext *fileCtx) -> std::vector<std::uint8_t>
{
    LOG_DEBUG("Getting file data");
    std::FILE *file = std::fopen(fileCtx->path.c_str(), "rb");

    if (file) {

        std::fseek(file, fileCtx->offset, SEEK_SET);

        auto dataLeft =
            std::min(static_cast<std::size_t>(FileOperations::ChunkSize), (fileCtx->size - fileCtx->offset));

        std::vector<std::uint8_t> buffer(dataLeft);

        auto dataRead = std::fread(buffer.data(), sizeof(uint8_t), dataLeft, file);

        std::fclose(file);

        LOG_DEBUG("Read %u bytes", static_cast<unsigned int>(dataRead));

        if (dataRead != dataLeft) {
            LOG_ERROR("File read error");
            buffer.clear();
            buffer.shrink_to_fit();
        }

        return buffer;
    }
    else {
        LOG_ERROR("File open error");
    }

    return {};
}

auto FileOperations::encodedSize(std::size_t binarySize) -> std::size_t
{
    /* 3 bytes of binary data is converted into 4 bytes of (printable) text
       returned as a string of characters including an additional 1 byte for null termination.
    */
    return ((binarySize + 2) / 3 * 4) + 1;
}

auto FileOperations::encodeDataAsBase64(const std::vector<std::uint8_t> &binaryData) -> std::string
{
    LOG_DEBUG("Encoding data to base64");

    auto encodedDataSize = encodedSize(binaryData.size());

    std::string encodedData(encodedDataSize, '\0');

    bintob64(encodedData.data(), binaryData.data(), binaryData.size());

    return encodedData;
}

auto FileOperations::validateChunkRequest(const FileContext *fileCtx, std::uint32_t chunkNo) -> bool
{
    auto totalChunks   = 1 + fileCtx->size / ChunkSize;
    auto expectedChunk = 1 + fileCtx->offset / ChunkSize;

    if (chunkNo < 1 || chunkNo > totalChunks || chunkNo != expectedChunk) {
        // Error, invalid input
        return false;
    }

    return true;
}

auto FileOperations::getDataForReceiveID(std::uint32_t rxID, std::uint32_t chunkNo) -> std::string
{
    LOG_DEBUG("Getting data for rxID %u", static_cast<unsigned>(rxID));

    auto fileCtxEntry = transfers.find(rxID);

    if (fileCtxEntry == transfers.end()) {
        LOG_ERROR("Invalid rxID %u", static_cast<unsigned>(rxID));
        return {};
    }

    auto fileCtx = fileCtxEntry->second.get();

    if (!validateChunkRequest(fileCtx, chunkNo)) {
        LOG_ERROR("Invalid chunkNo %u", static_cast<unsigned>(chunkNo));
        return {};
    }

    auto data = getDataForFileCtx(fileCtx);

    if (data.empty()) {
        LOG_ERROR("File %s read error", fileCtx->path.c_str());
        return {};
    }

    fileCtx->offset += data.size();

    if (fileCtx->offset >= fileCtx->size) {
        LOG_INFO("Reached EOF of %s", fileCtx->path.c_str());
        transfers.erase(rxID);
    }

    return encodeDataAsBase64(data);
}
