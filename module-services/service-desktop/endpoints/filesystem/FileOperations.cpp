// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FileOperations.hpp"
#include "FileContext.cpp"
#include <base64.h>
#include <log.hpp>

FileOperations &FileOperations::instance()
{
    static FileOperations instance;
    return instance;
}

auto FileOperations::createReceiveIDForFile(const std::filesystem::path &file) -> std::pair<transfer_id, std::size_t>
{
    cancelTimedOutTransfer();
    const auto rxID = ++runningRxId;
    const auto size = std::filesystem::file_size(file);

    if (!size) {
        LOG_ERROR("File %s is empty", file.c_str());
        return std::make_pair(0, 0);
    }

    LOG_DEBUG("Creating rxID %u", static_cast<unsigned>(rxID));

    try {
        createFileContextFor(file, size, rxID);
    }
    catch (std::exception &e) {
        LOG_ERROR("FileOperations::createFileContextFor() exception: %s", e.what());
        return std::make_pair(0, 0);
    }

    return std::make_pair(rxID, size);
}

void FileOperations::cancelTimedOutTransfer()
{
    if (!runningRxId) {
        return;
    }

    auto timedOutXfer       = runningRxId - 1;
    const auto fileCtxEntry = transfers.find(timedOutXfer);

    if (fileCtxEntry == transfers.end()) {
        LOG_DEBUG("No timed out transfers");
        return;
    }

    LOG_DEBUG("Canceling timed out rxID %u", static_cast<unsigned>(timedOutXfer));
    transfers.erase(timedOutXfer);
}

auto FileOperations::createFileContextFor(const std::filesystem::path &file, std::size_t fileSize, transfer_id rxId)
    -> void
{
    transfers.insert(std::make_pair(rxId, std::make_unique<FileContext>(file, fileSize, 0, FileOperations::ChunkSize)));
}

auto FileOperations::encodedSize(std::size_t binarySize) const -> std::size_t
{
    /* 3 bytes of binary data is converted into 4 bytes of (printable) text
       returned as a string of characters including an additional 1 byte for null termination.
    */
    return ((binarySize + Mod3MaxReminder) / Base64ToBinFactor * BinToBase64Factor) + 1;
}

auto FileOperations::encodeDataAsBase64(const std::vector<std::uint8_t> &binaryData) const -> std::string
{
    const auto encodedDataSize = encodedSize(binaryData.size());

    std::string encodedData(encodedDataSize, '\0');

    bintob64(encodedData.data(), binaryData.data(), binaryData.size());

    return encodedData;
}

auto FileOperations::getDataForReceiveID(transfer_id rxID, std::uint32_t chunkNo) -> std::string
{
    LOG_DEBUG("Getting data for rxID %u", static_cast<unsigned>(rxID));

    const auto fileCtxEntry = transfers.find(rxID);

    if (fileCtxEntry == transfers.end()) {
        LOG_ERROR("Invalid rxID %u", static_cast<unsigned>(rxID));
        return {};
    }

    auto fileCtx = fileCtxEntry->second.get();

    if (!fileCtx) {
        LOG_ERROR("Invalid fileCtx for rxID %u", static_cast<unsigned>(rxID));
        return {};
    }

    if (!fileCtx->validateChunkRequest(chunkNo)) {
        LOG_ERROR("Invalid chunkNo %u", static_cast<unsigned>(chunkNo));
        return {};
    }

    const auto data = fileCtx->getDataForFile();

    if (data.empty()) {
        LOG_ERROR("File read error");
        return {};
    }

    if (fileCtx->reachedEOF()) {
        LOG_INFO("Reached EOF for rxID %u", static_cast<unsigned>(rxID));
        transfers.erase(rxID);
    }

    return encodeDataAsBase64(data);
}
