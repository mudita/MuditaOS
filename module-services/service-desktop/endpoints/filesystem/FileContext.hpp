// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <filesystem>
#include <vector>
#include <map>
#include <atomic>

struct FileContext
{
    explicit FileContext(std::filesystem::path path, std::size_t size, std::size_t offset, std::size_t chunkSize);

    auto validateChunkRequest(std::uint32_t chunkNo) const -> bool;

    auto advanceFileOffset(std::size_t bySize) -> void;

    auto reachedEOF() const -> bool;

    auto chunksInQuantity(std::size_t quantity) const -> std::size_t;

    auto totalChunksInFile() const -> std::size_t;

    auto expectedChunkInFile() const -> std::size_t;

    auto getDataForFile() -> std::vector<std::uint8_t>;

  private:
    std::filesystem::path path{};
    std::size_t size{};
    std::size_t offset{};
    std::size_t chunkSize{};
};
