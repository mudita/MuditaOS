// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "messages/FileIndexerMessages.hpp"
#include "agents/DatabaseAgent.hpp"

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
    auto handleListDir(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleGetProperty(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleGetAllProperties(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleSetProperty(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleSetProperties(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleGetRecord(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleSetRecord(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
};
