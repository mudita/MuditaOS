// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DatabaseAgent.hpp>
#include <service-db/FileIndexerMessages.hpp>

class FileIndexerAgent : public DatabaseAgent
{
  public:
    FileIndexerAgent(sys::Service *parentService);
    ~FileIndexerAgent() = default;

    void initDb() override;
    void deinitDb() override;
    void registerMessages() override;
    auto getAgentName() -> const std::string override;
    void dbTest();

  private:
    auto dbListDir(std::unique_ptr<FileIndexer::ListDirData> listDir) -> std::unique_ptr<FileIndexer::ListDirData>;
    auto dbGetProperty(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> FileIndexer::FileMetadata;
    auto dbSetProperty(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool;
    auto dbSetProperties(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool;
    auto dbUpdateProperties(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool;
    auto dbGetAllProperties(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> FileIndexer::FileMetadata;
    auto dbGetRecord(std::unique_ptr<FileIndexer::FileRecord> record) -> FileIndexer::FileRecord;
    auto dbSetRecord(std::unique_ptr<FileIndexer::FileRecord> record) -> bool;
    auto dbUpdateRecord(std::unique_ptr<FileIndexer::FileRecord> record) -> bool;
    auto getDbInitString() -> const std::string override;
    auto getDbFilePath() -> const std::string override;
    auto dbGetFilesCount() -> unsigned int;
    auto handleListDir(sys::Message *req) -> sys::MessagePointer;
    auto handleGetProperty(sys::Message *req) -> sys::MessagePointer;
    auto handleGetAllProperties(sys::Message *req) -> sys::MessagePointer;
    auto handleSetProperty(sys::Message *req) -> sys::MessagePointer;
    auto handleSetProperties(sys::Message *req) -> sys::MessagePointer;
    auto handleGetRecord(sys::Message *req) -> sys::MessagePointer;
    auto handleSetRecord(sys::Message *req) -> sys::MessagePointer;
};
