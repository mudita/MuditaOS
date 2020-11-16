// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FileIndexerAgent.hpp"
#include "FileIndexer_queries.hpp"

#include <log/log.hpp>
#include <module-sys/Service/Bus.hpp>
#include <vfs.hpp>

#include <memory>

FileIndexerAgent::FileIndexerAgent(sys::Service *parentService) : DatabaseAgent(parentService)
{
    database = std::make_unique<Database>(getDbFilePath().c_str());
}

void FileIndexerAgent::initDb()
{
    LOG_INFO("[ServiceDB][File Indexer] Initializing");
    database->execute(getDbInitString().c_str());
}

void FileIndexerAgent::deinitDb()
{
    database->deinitialize();
}

void FileIndexerAgent::registerMessages()
{
    // connect handler & message in parent service
    using std::placeholders::_1;

    // all API asynchronic
    parentService->connect(FileIndexer::Messages::GetListDirMessage(),
                           std::bind(&FileIndexerAgent::handleListDir, this, _1));

    parentService->connect(FileIndexer::Messages::GetRecordMessage(),
                           std::bind(&FileIndexerAgent::handleGetRecord, this, _1));
    parentService->connect(FileIndexer::Messages::SetRecordMessage(),
                           std::bind(&FileIndexerAgent::handleSetRecord, this, _1));

    parentService->connect(FileIndexer::Messages::GetPropertyMessage(),
                           std::bind(&FileIndexerAgent::handleGetProperty, this, _1));
    parentService->connect(FileIndexer::Messages::SetPropertyMessage(),
                           std::bind(&FileIndexerAgent::handleSetProperty, this, _1));

    parentService->connect(FileIndexer::Messages::GetAllPropertiesMessage(),
                           std::bind(&FileIndexerAgent::handleGetAllProperties, this, _1));
    parentService->connect(FileIndexer::Messages::SetPropertiesMessage(),
                           std::bind(&FileIndexerAgent::handleSetProperties, this, _1));
}

auto FileIndexerAgent::getDbInitString() -> const std::string
{
    const char *sql = (""
#include "file_indexer.sql"
    );
    return sql;
}

auto FileIndexerAgent::getDbFilePath() -> const std::string
{

    return USER_PATH("file_indexer.db");
}
auto FileIndexerAgent::getAgentName() -> const std::string
{
    return std::string("fileIndexerAgent");
}

auto FileIndexerAgent::dbGetFilesCount() -> unsigned int
{
    auto retQuery = database->query(FileIndexer::Statements::getFilesCount);
    if (nullptr == retQuery || 1 != retQuery->getRowCount()) {
        return 0;
    }
    return (*retQuery)[0].getInt32();
}

auto FileIndexerAgent::dbListDir(std::unique_ptr<FileIndexer::ListDirData> listDir)
    -> std::unique_ptr<FileIndexer::ListDirData>
{

    auto retQuery = database->query(FileIndexer::Statements::getFilesByDir, listDir->directory.c_str());
    if (nullptr == retQuery) {
        listDir->count = 0;
        return listDir;
    }

    listDir->count = retQuery->getRowCount();
    if (retQuery->getRowCount() > 0) {
        listDir->fileList.clear();
        do {
            listDir->fileList.push_back(FileIndexer::FileRecord(retQuery.get()));
        } while (retQuery->nextRow());
    }
    return listDir;
}

auto FileIndexerAgent::handleListDir(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::GetListDirMessage *>(req)) {
        return std::make_shared<FileIndexer::Messages::GetListDirResponseMessage>(dbListDir(std::move(msg->listDir)));
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto FileIndexerAgent::dbGetRecord(std::unique_ptr<FileIndexer::FileRecord> record) -> FileIndexer::FileRecord
{
    std::unique_ptr<QueryResult> retQuery = nullptr;

    if (false == record->path.empty()) {
        retQuery = database->query(FileIndexer::Statements::getFileInfoByPath, record->path.c_str());
        if (nullptr == retQuery || 1 != retQuery->getRowCount()) {
            return FileIndexer::FileRecord{};
        }
    }
    else if (record->file_id != FileIndexer::FILE_ID_NOT_EXISTS) {
        retQuery = database->query(FileIndexer::Statements::getFileInfoById, record->file_id);
        if (nullptr == retQuery || 1 != retQuery->getRowCount()) {
            return FileIndexer::FileRecord{};
        }
    }
    else {
        return FileIndexer::FileRecord{};
    }
    return FileIndexer::FileRecord(retQuery.get());
}

auto FileIndexerAgent::dbSetRecord(std::unique_ptr<FileIndexer::FileRecord> record) -> bool
{
    return database->execute(FileIndexer::Statements::insertFileInfo,
                             record->file_id,
                             record->path.c_str(),
                             record->size,
                             record->mime_type,
                             record->mtime,
                             record->directory.c_str(),
                             record->file_type);
}

auto FileIndexerAgent::dbUpdateRecord(std::unique_ptr<FileIndexer::FileRecord> record) -> bool
{
    return database->execute(FileIndexer::Statements::updateFileInfo,
                             record->path.c_str(),
                             record->size,
                             record->mime_type,
                             record->mtime,
                             record->directory.c_str(),
                             record->file_type,
                             record->file_id);
}

auto FileIndexerAgent::handleGetRecord(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::GetRecordMessage *>(req)) {
        auto record   = std::move(msg->record);
        msg->dbRecord = dbGetRecord(std::move(record));
        return std::make_shared<FileIndexer::Messages::GetRecordResponseMessage>(
            std::make_unique<FileIndexer::FileRecord>(msg->dbRecord));
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto FileIndexerAgent::handleSetRecord(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::SetRecordMessage *>(req)) {
        auto recordPtr                 = std::move(msg->record);
        FileIndexer::FileRecord record = *recordPtr;
        msg->dbRecord                  = dbGetRecord(std::make_unique<FileIndexer::FileRecord>(record));

        if (!(msg->dbRecord.RecordEqual(record))) {
            auto updateMsg = std::make_shared<FileIndexer::Messages::RecordChangedMessage>(
                std::move(recordPtr), std::make_unique<FileIndexer::FileRecord>(msg->dbRecord));

            sys::Bus::SendUnicast(std::move(updateMsg), msg->sender, parentService);
            dbSetRecord(std::make_unique<FileIndexer::FileRecord>(record));
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto FileIndexerAgent::dbGetProperty(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> FileIndexer::FileMetadata
{
    auto itr      = metaData->properties.begin();
    auto property = itr->first;

    std::unique_ptr<QueryResult> retQuery = nullptr;

    retQuery = database->query(FileIndexer::Statements::getPropertyValue, property.c_str(), metaData->path.c_str());
    if (nullptr == retQuery || 1 != retQuery->getRowCount()) {
        return *metaData;
    }
    FileIndexer::FileMetadata retMetaData;
    retMetaData.file_id = (*retQuery)[0].getInt32();
    retMetaData.properties.clear();
    retMetaData.properties.emplace((*retQuery)[1].getString(), (*retQuery)[2].getString());
    return retMetaData;
}

auto FileIndexerAgent::dbGetAllProperties(std::unique_ptr<FileIndexer::FileMetadata> metaData)
    -> FileIndexer::FileMetadata
{
    std::unique_ptr<QueryResult> retQuery = nullptr;
    FileIndexer::FileMetadata retMetaData = {};

    retQuery = database->query(FileIndexer::Statements::getAllProperties, metaData->path.c_str());
    if (nullptr == retQuery) {
        return retMetaData;
    }
    if (retQuery->getRowCount() > 0) {
        retMetaData.file_id = (*retQuery)[0].getInt32();
        retMetaData.properties.clear();
        do {
            retMetaData.properties.emplace((*retQuery)[1].getString(), (*retQuery)[2].getString());
        } while (retQuery->nextRow());
    }
    return retMetaData;
}

auto FileIndexerAgent::dbSetProperty(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool
{
    auto itr      = metaData->properties.begin();
    auto property = itr->first;
    auto value    = itr->second;
    return database->execute(
        FileIndexer::Statements::insertPropertyValue, metaData->file_id, property.c_str(), value.c_str());
}

auto FileIndexerAgent::dbSetProperties(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool
{
    bool statusCode = true;
    for (auto propVal : metaData->properties) {
        statusCode = database->execute(FileIndexer::Statements::insertPropertyValue,
                                       metaData->file_id,
                                       propVal.first.c_str(),
                                       propVal.second.c_str());
        if (!statusCode)
            return statusCode;
    }
    return statusCode;
}

auto FileIndexerAgent::dbUpdateProperties(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool
{
    bool statusCode = true;
    for (auto propVal : metaData->properties) {
        statusCode = database->execute(FileIndexer::Statements::updatePropertyValue,
                                       propVal.second.c_str(),
                                       metaData->file_id,
                                       propVal.first.c_str());
        if (!statusCode)
            return statusCode;
    }
    return statusCode;
}

auto FileIndexerAgent::handleGetProperty(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::GetPropertyMessage *>(req)) {
        auto metaDataPtr                   = std::move(msg->metaData);
        FileIndexer::FileMetadata metaData = *metaDataPtr;
        msg->dbMetaData                    = dbGetProperty(std::make_unique<FileIndexer::FileMetadata>(metaData));
        return std::make_shared<FileIndexer::Messages::GetPropertyResponseMessage>(
            std::make_unique<FileIndexer::FileMetadata>(msg->dbMetaData));
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto FileIndexerAgent::handleGetAllProperties(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::GetPropertyMessage *>(req)) {
        auto metaDataPtr                   = std::move(msg->metaData);
        FileIndexer::FileMetadata metaData = *metaDataPtr;
        msg->dbMetaData                    = dbGetAllProperties(std::make_unique<FileIndexer::FileMetadata>(metaData));
        return std::make_shared<FileIndexer::Messages::GetPropertyResponseMessage>(
            std::make_unique<FileIndexer::FileMetadata>(msg->dbMetaData));
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto FileIndexerAgent::handleSetProperty(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::SetPropertyMessage *>(req)) {
        auto metaDataPtr                   = std::move(msg->metaData);
        FileIndexer::FileMetadata metaData = *metaDataPtr;

        auto itr   = metaDataPtr->properties.begin();
        auto value = itr->second;

        msg->dbMetaData = dbGetProperty(std::make_unique<FileIndexer::FileMetadata>(metaData));
        auto dbItr      = msg->dbMetaData.properties.begin();
        auto dbValue    = dbItr->second;

        if (dbValue != value) {
            auto updateMsg = std::make_shared<FileIndexer::Messages::PropertyChangedMessage>(
                std::move(metaDataPtr), std::make_unique<FileIndexer::FileMetadata>(msg->dbMetaData));
            sys::Bus::SendUnicast(std::move(updateMsg), msg->sender, parentService);
            dbSetProperty(std::make_unique<FileIndexer::FileMetadata>(metaData));
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto FileIndexerAgent::handleSetProperties(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::SetPropertyMessage *>(req)) {
        auto metaDataPtr                   = std::move(msg->metaData);
        FileIndexer::FileMetadata metaData = *metaDataPtr;
        msg->dbMetaData                    = dbGetAllProperties(std::make_unique<FileIndexer::FileMetadata>(metaData));
        auto updateMsg                     = std::make_shared<FileIndexer::Messages::PropertyChangedMessage>(
            std::move(metaDataPtr), std::make_unique<FileIndexer::FileMetadata>(msg->dbMetaData));
        sys::Bus::SendUnicast(std::move(updateMsg), msg->sender, parentService);
        dbSetProperty(std::make_unique<FileIndexer::FileMetadata>(metaData));
    }
    return std::make_shared<sys::ResponseMessage>();
}
