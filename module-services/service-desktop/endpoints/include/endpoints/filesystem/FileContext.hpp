// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "system/SystemReturnCodes.hpp"
#include <crc32.h>

#include <filesystem>
#include <vector>
#include <map>
#include <atomic>
#include <fstream>
#include <iostream>

class FileContext
{
  public:
    explicit FileContext(const std::filesystem::path &path,
                         std::size_t size,
                         std::size_t chunkSize,
                         std::size_t offset = 0);

    virtual ~FileContext();

    auto validateChunkRequest(std::uint32_t chunkNo) const -> bool;

    auto advanceFileOffset(std::size_t bySize) -> void;

    auto reachedEOF() const -> bool;

    auto chunksInQuantity(std::size_t quantity) const -> std::size_t;

    auto totalChunksInFile() const -> std::size_t;

    auto expectedChunkInFile() const -> std::size_t;

    auto fileHash() const -> std::string;

  protected:
    std::filesystem::path path{};
    std::size_t size{};
    std::size_t offset{};
    std::size_t chunkSize{};
    CRC32 runningCrc32Digest;
};

class FileReadContext : public FileContext
{
  public:
    explicit FileReadContext(const std::filesystem::path &path,
                             std::size_t size,
                             std::size_t chunkSize,
                             std::size_t offset = 0);

    ~FileReadContext();

    auto read() -> std::vector<std::uint8_t>;
};

class FileWriteContext : public FileContext
{
  public:
    explicit FileWriteContext(const std::filesystem::path &path,
                              std::size_t size,
                              std::size_t chunkSize,
                              std::string crc32Digest,
                              std::size_t offset = 0);

    ~FileWriteContext();

    auto crc32Matches() const -> bool;

    auto removeFile() -> void;

    auto write(const std::vector<std::uint8_t> &data) -> void;

  private:
    std::string crc32Digest{};

    std::ofstream file{};
};
