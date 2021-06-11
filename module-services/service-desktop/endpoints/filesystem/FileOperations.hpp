// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

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

    struct FileContext
    {
        explicit FileContext(std::filesystem::path path, std::size_t size, std::size_t offset)
            : path(path), size(size), offset(offset){};

        std::filesystem::path path;
        std::size_t size;
        std::size_t offset;
    };

    std::map<std::uint32_t, std::unique_ptr<FileContext>> transfers;

    std::atomic<std::uint32_t> runningRxId = 0;
    std::atomic<std::uint32_t> runningTxId = 0;

    auto createFileContextFor(const std::filesystem::path &file, std::size_t fileSize, std::uint32_t rxId) -> void;

    auto validateChunkRequest(const FileContext *fileCtx, std::uint32_t chunkNo) -> bool;

    auto getDataForFileCtx(const FileContext *fileCtx) -> std::vector<std::uint8_t>;

    auto encodeDataAsBase64(const std::vector<std::uint8_t> &binaryData) -> std::string;

    auto encodedSize(std::size_t binarySize) -> std::size_t;

  public:
    enum : std::size_t
    {
        ChunkSize = 3 * 1024
    };

    static FileOperations &instance();

    auto createReceiveIDForFile(const std::filesystem::path &file) -> std::pair<std::uint32_t, std::size_t>;

    auto getDataForReceiveID(std::uint32_t rxID, std::uint32_t chunkNo) -> std::string;
};
