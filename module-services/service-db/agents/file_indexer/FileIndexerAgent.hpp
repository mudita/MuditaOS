// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DatabaseAgent.hpp>
#include <service-db/FileIndexerMessages.hpp>

#include <set>

class FileIndexerAgent : public DatabaseAgent
{
  public:
    FileIndexerAgent(sys::Service *parentService);
    ~FileIndexerAgent() = default;

    void initDb() override;
    void deinitDb() override;
    void registerMessages() override;
    auto getAgentName() -> const std::string override;

  protected:
    // msg handlers
    auto handleRegisterOnFileChange(sys::Message *req) -> sys::MessagePointer;
    auto handleUnregisterOnFileChange(sys::Message *req) -> sys::MessagePointer;
    auto handleListDir(sys::Message *req) -> sys::MessagePointer;
    auto handleGetProperty(sys::Message *req) -> sys::MessagePointer;
    auto handleGetAllProperties(sys::Message *req) -> sys::MessagePointer;
    auto handleSetProperty(sys::Message *req) -> sys::MessagePointer;
    auto handleSetProperties(sys::Message *req) -> sys::MessagePointer;
    auto handleGetRecord(sys::Message *req) -> sys::MessagePointer;
    auto handleSetRecord(sys::Message *req) -> sys::MessagePointer;
    auto handleDeleteFile(sys::Message *req) -> sys::MessagePointer;
    auto handleDeleteAllFilesInDir(sys::Message *req) -> sys::MessagePointer;

    using MapOfRecipentsToBeNotified = std::map<std::string, std::set<std::string>>;
    MapOfRecipentsToBeNotified fileChangeRecipents;

  private:
    // db operations
    auto dbRegisterFileChange(std::string dir, std::string service) -> bool;
    auto dbUnregisterFileChange(std::string dir, std::string service) -> bool;
    auto dbListDir(unsigned int offset, unsigned int limit, const std::string &directory)
        -> std::unique_ptr<FileIndexer::ListDir>;
    auto dbGetProperty(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> FileIndexer::FileMetadata;
    auto dbSetProperty(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool;
    auto dbSetProperties(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool;
    auto dbUpdateProperties(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool;
    auto dbGetAllProperties(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> FileIndexer::FileMetadata;
    auto dbGetRecord(std::unique_ptr<FileIndexer::FileRecord> record) -> FileIndexer::FileRecord;
    auto dbSetRecord(std::unique_ptr<FileIndexer::FileRecord> record) -> bool;
    auto dbDeleteFile(std::unique_ptr<FileIndexer::FileRecord> record) -> bool;
    auto dbDeleteAllFilesInDir(std::unique_ptr<FileIndexer::FileRecord> record) -> bool;
    auto dbUpdateRecord(std::unique_ptr<FileIndexer::FileRecord> record) -> bool;
    auto getDbInitString() -> const std::string override;
    auto getDbFilePath() -> const std::string override;
    auto dbGetFilesCount() -> unsigned int;
    auto dbGeMetadataCount() -> unsigned int;
};
