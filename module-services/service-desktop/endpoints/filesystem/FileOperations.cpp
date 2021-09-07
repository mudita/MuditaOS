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
    cancelTimedOutReadTransfer();
    const auto rxID = ++runningXfrId;
    const auto size = std::filesystem::file_size(file);

    if (size == 0) {
        LOG_ERROR("File %s is empty", file.c_str());
        throw std::runtime_error("File size error");
    }

    LOG_DEBUG("Creating rxID %u", static_cast<unsigned>(rxID));

    createFileReadContextFor(file, size, rxID);

    return std::make_pair(rxID, size);
}

void FileOperations::cancelTimedOutReadTransfer()
{
    if (!runningXfrId) {
        return;
    }

    auto timedOutXfer       = runningXfrId - 1;
    const auto fileCtxEntry = readTransfers.find(timedOutXfer);

    if (fileCtxEntry == readTransfers.end()) {
        LOG_DEBUG("No timed out transfers");
        return;
    }

    LOG_DEBUG("Canceling timed out rxID %u", static_cast<unsigned>(timedOutXfer));
    readTransfers.erase(timedOutXfer);
}

void FileOperations::cancelTimedOutWriteTransfer()
{
    if (!runningXfrId) {
        return;
    }

    auto timedOutXfer       = runningXfrId - 1;
    const auto fileCtxEntry = writeTransfers.find(timedOutXfer);

    if (fileCtxEntry == writeTransfers.end()) {
        LOG_DEBUG("No timed out transfers");
        return;
    }

    LOG_DEBUG("Canceling timed out rxID %u", static_cast<unsigned>(timedOutXfer));
    writeTransfers.erase(timedOutXfer);
}

auto FileOperations::createFileReadContextFor(const std::filesystem::path &file,
                                              std::size_t fileSize,
                                              transfer_id xfrId) -> void
{
    readTransfers.insert(
        std::make_pair(xfrId, std::make_unique<FileReadContext>(file, fileSize, FileOperations::ChunkSize)));
}

auto FileOperations::createFileWriteContextFor(const std::filesystem::path &file,
                                               std::size_t fileSize,
                                               const std::string Crc32,
                                               transfer_id xfrId) -> void
{
    writeTransfers.insert(
        std::make_pair(xfrId, std::make_unique<FileWriteContext>(file, fileSize, FileOperations::ChunkSize, Crc32)));
}

auto FileOperations::encodedSize(std::size_t binarySize) const -> std::size_t
{
    /* 3 bytes of binary data is converted into 4 bytes of (printable) text
       returned as a string of characters including an additional 1 byte for null termination.
    */
    return ((binarySize + Mod3MaxReminder) / Base64ToBinFactor * BinToBase64Factor) + 1;
}

auto FileOperations::decodedSize(std::size_t encodedSize) const -> std::size_t
{
    /* 4 bytes of encoded data is converted back into 3 bytes of binary
        ((bytes + 3) / 4) * 3;
    */
    return ((((encodedSize + Base64ToBinFactor) / BinToBase64Factor) * Base64ToBinFactor) + 1);
}

auto FileOperations::encodeDataAsBase64(const std::vector<std::uint8_t> &binaryData) const -> std::string
{
    const auto encodedDataSize = encodedSize(binaryData.size());

    std::string encodedData(encodedDataSize, '\0');

    bintob64(encodedData.data(), binaryData.data(), binaryData.size());

    return encodedData;
}

auto FileOperations::decodeDataFromBase64(const std::string &encodedData) const -> std::vector<std::uint8_t>
{
    const auto decodedDataSize = decodedSize(encodedData.length());

    std::vector<std::uint8_t> decodedData(decodedDataSize, 0);

    b64tobin(decodedData.data(), encodedData.data());

    return decodedData;
}

auto FileOperations::getFileHashForReceiveID(transfer_id rxID) -> std::string
{
    LOG_DEBUG("Getting hash for rxID %u", static_cast<unsigned>(rxID));

    const auto fileCtxEntry = readTransfers.find(rxID);

    if (fileCtxEntry == readTransfers.end()) {
        LOG_ERROR("Invalid rxID %u", static_cast<unsigned>(rxID));
        return {};
    }

    auto fileCtx = fileCtxEntry->second.get();

    if (!fileCtx) {
        LOG_ERROR("Invalid fileCtx for rxID %u", static_cast<unsigned>(rxID));
        return {};
    }

    return fileCtx->getFileHash();
}

auto FileOperations::getDataForReceiveID(transfer_id rxID, std::uint32_t chunkNo) -> std::string
{
    LOG_DEBUG("Getting data for rxID %u", static_cast<unsigned>(rxID));

    const auto fileCtxEntry = readTransfers.find(rxID);

    if (fileCtxEntry == readTransfers.end()) {
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

    const auto data = fileCtx->read();

    if (data.empty()) {
        LOG_ERROR("File read error");
        return {};
    }

    if (fileCtx->reachedEOF()) {
        LOG_INFO("Reached EOF for rxID %u", static_cast<unsigned>(rxID));

        writeTransfers.erase(rxID);
    }

    return encodeDataAsBase64(data);
}

auto FileOperations::createTransmitIDForFile(const std::filesystem::path &file,
                                             std::size_t size,
                                             const std::string Crc32) -> transfer_id
{
    cancelTimedOutWriteTransfer();
    const auto txID = ++runningXfrId;

    LOG_DEBUG("Creating rxID %u", static_cast<unsigned>(txID));

    createFileWriteContextFor(file, size, Crc32, txID);

    return txID;
}

auto FileOperations::sendDataForTransmitID(transfer_id txID, std::uint32_t chunkNo, const std::string &data)
    -> sys::ReturnCodes
{
    LOG_DEBUG("Transmitting data for txID %u", static_cast<unsigned>(txID));
    auto returnCode = sys::ReturnCodes::Success;

    const auto fileCtxEntry = writeTransfers.find(txID);

    if (fileCtxEntry == writeTransfers.end()) {
        LOG_ERROR("Invalid txID %u", static_cast<unsigned>(txID));
        return sys::ReturnCodes::Failure;
    }

    auto fileCtx = fileCtxEntry->second.get();

    if (!fileCtx) {
        LOG_ERROR("Invalid fileCtx for txID %u", static_cast<unsigned>(txID));
        return sys::ReturnCodes::Failure;
    }

    if (!fileCtx->validateChunkRequest(chunkNo)) {
        LOG_ERROR("Invalid chunkNo %u", static_cast<unsigned>(chunkNo));
        return sys::ReturnCodes::Failure;
    }

    auto binaryData = decodeDataFromBase64(data);

    fileCtx->write(binaryData);

    if (fileCtx->reachedEOF()) {
        LOG_INFO("Reached EOF for txID %u", static_cast<unsigned>(txID));

        auto fileOK = fileCtx->crc32Matches();

        if (!fileOK) {
            LOG_ERROR("File CRC32 mismatch");
            fileCtx->removeFile();
            writeTransfers.erase(txID);

            throw std::runtime_error("File CRC32 mismatch");
        }
        writeTransfers.erase(txID);
    }

    return returnCode;
}
