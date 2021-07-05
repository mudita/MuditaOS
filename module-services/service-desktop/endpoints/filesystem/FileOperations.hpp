// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FileContext.hpp"

#include <filesystem>
#include <vector>
#include <map>
#include <atomic>

class FileOperations
{
    FileOperations()  = default;
    ~FileOperations() = default;

    FileOperations(const FileOperations &) = delete;
    FileOperations &operator=(const FileOperations &) = delete;

    using transfer_id = std::uint32_t;

    std::map<transfer_id, std::unique_ptr<FileContext>> transfers;

    std::atomic<transfer_id> runningRxId{0};

    auto createFileContextFor(const std::filesystem::path &file, std::size_t fileSize, transfer_id rxId) -> void;

    auto encodeDataAsBase64(const std::vector<std::uint8_t> &binaryData) const -> std::string;

    auto encodedSize(std::size_t binarySize) const -> std::size_t;

    auto cancelTimedOutTransfer() -> void;

  public:
    static constexpr auto ChunkSize         = 3 * 1024u;
    static constexpr auto BinToBase64Factor = 4u;
    static constexpr auto Base64ToBinFactor = 3u;
    static constexpr auto Mod3MaxReminder   = 2u;

    static FileOperations &instance();

    auto createReceiveIDForFile(const std::filesystem::path &file) -> std::pair<transfer_id, std::size_t>;

    auto getDataForReceiveID(transfer_id, std::uint32_t chunkNo) -> std::string;
};
