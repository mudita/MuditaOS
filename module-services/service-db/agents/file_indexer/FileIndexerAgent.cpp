// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FileIndexerAgent.hpp"
#include "FileIndexer_queries.hpp"

#include <log.hpp>
#include <purefs/filesystem_paths.hpp>
#include <Application.hpp>
#include <memory>

FileIndexerAgent::FileIndexerAgent(sys::Service *parentService) : DatabaseAgent(parentService)
{
    database = std::make_unique<Database>(getDbFilePath().c_str());
}

void FileIndexerAgent::initDb()
{
    auto notifications = database->query(FileIndexer::Statements::getAllNotifications);
    if (nullptr == notifications || DatabaseAgent::ONE_ROW_FOUND == notifications->getRowCount()) {
        return;
    }
    if (notifications->getFieldCount() == FileIndexer::NOTIFICATION_RECORD_COLUMN_COUNT) {
        fileChangeRecipents.clear();
        do {
            std::string directory = (*notifications)[1].getString();
            std::string service   = (*notifications)[2].getString();
            fileChangeRecipents[directory].insert(service);
        } while (notifications->nextRow());
    }
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
    parentService->connect(FileIndexer::Messages::GetListDirRequest(),
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

    parentService->connect(FileIndexer::Messages::DeleteFileMessage(),
                           std::bind(&FileIndexerAgent::handleDeleteFile, this, _1));
    parentService->connect(FileIndexer::Messages::DeleteAllFilesInDirMessage(),
                           std::bind(&FileIndexerAgent::handleDeleteAllFilesInDir, this, _1));

    parentService->connect(FileIndexer::Messages::RegisterOnFileChange(),
                           std::bind(&FileIndexerAgent::handleRegisterOnFileChange, this, _1));
    parentService->connect(FileIndexer::Messages::UnregisterOnFileChange(),
                           std::bind(&FileIndexerAgent::handleUnregisterOnFileChange, this, _1));
}

auto FileIndexerAgent::getDbInitString() -> const std::string
{
    return {};
}

auto FileIndexerAgent::getDbFilePath() -> const std::string
{
    return (purefs::dir::getUserDiskPath() / "file_indexer.db").string();
}
auto FileIndexerAgent::getAgentName() -> const std::string
{
    return std::string("fileIndexerAgent");
}

auto FileIndexerAgent::dbRegisterFileChange(std::string dir, std::string service) -> bool
{
    auto retQuery = database->query(FileIndexer::Statements::getNotification, dir.c_str(), service.c_str());
    if (nullptr == retQuery || DatabaseAgent::ZERO_ROWS_FOUND == retQuery->getRowCount()) {
        // notification does not exist in db, insert a new one
        return database->execute(FileIndexer::Statements::setNotification, dir.c_str(), service.c_str());
    }
    // service already registered for the dir
    return false;
}

auto FileIndexerAgent::dbUnregisterFileChange(std::string dir, std::string service) -> bool
{
    return database->execute(FileIndexer::Statements::clearNotificationdRow, dir.c_str(), service.c_str());
}

auto FileIndexerAgent::handleRegisterOnFileChange(sys::Message *req) -> sys::MessagePointer
{
    std::string directory;
    if (auto msg = dynamic_cast<FileIndexer::Messages::RegisterOnFileChange *>(req)) {
        if (msg->directory != nullptr) {
            directory = *(msg->directory);
            if (dbRegisterFileChange(directory, msg->sender)) {
                auto it = fileChangeRecipents.find(directory);
                if (fileChangeRecipents.end() == it) {
                    fileChangeRecipents[directory] = {msg->sender};
                }
                else {
                    it->second.insert(msg->sender);
                }
            }
        }
    }
    LOG_INFO("RegisterOnFileChange from %s", req->sender.c_str());
    return std::make_shared<sys::ResponseMessage>();
}

auto FileIndexerAgent::handleUnregisterOnFileChange(sys::Message *req) -> sys::MessagePointer
{
    std::string directory;
    if (auto msg = dynamic_cast<FileIndexer::Messages::UnregisterOnFileChange *>(req)) {
        if (msg->directory != nullptr) {
            directory = *(msg->directory);
            if (dbUnregisterFileChange(directory, msg->sender)) {
                auto it = fileChangeRecipents.find(directory);
                if (fileChangeRecipents.end() != it) {
                    it->second.erase(msg->sender);
                }
            }
        }
    }
    LOG_INFO("UnRegisterOnFileChange from %s", req->sender.c_str());
    return std::make_shared<sys::ResponseMessage>();
}

auto FileIndexerAgent::dbGetFilesCount() -> unsigned int
{
    auto retQuery = database->query(FileIndexer::Statements::getFilesCount);
    if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
        return 0;
    }
    return (*retQuery)[0].getUInt32();
}

auto FileIndexerAgent::dbGeMetadataCount() -> unsigned int
{
    auto retQuery = database->query(FileIndexer::Statements::getMetadataCount);
    if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
        return 0;
    }
    return (*retQuery)[0].getUInt32();
}

auto FileIndexerAgent::dbListDir(unsigned int offset, unsigned int limit, const std::string &directory)
    -> std::unique_ptr<FileIndexer::ListDir>
{
    auto retQuery = database->query(FileIndexer::Statements::getFilesByDir, directory.c_str());
    return getList<FileIndexer::ListDir, FileIndexer::FileRecord>(offset, limit, std::move(retQuery));
}

auto FileIndexerAgent::handleListDir(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::GetListDirRequest *>(req)) {
        return std::make_shared<FileIndexer::Messages::GetListDirResponse>(
            dbListDir(msg->offset, msg->limit, msg->directory));
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto FileIndexerAgent::dbGetRecord(std::unique_ptr<FileIndexer::FileRecord> record) -> FileIndexer::FileRecord
{
    std::unique_ptr<QueryResult> retQuery = nullptr;

    if (false == record->path.empty()) {
        retQuery = database->query(FileIndexer::Statements::getFileInfoByPath, record->path.c_str());
        if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
            return FileIndexer::FileRecord{};
        }
    }
    else if (record->file_id != FileIndexer::FILE_ID_NOT_EXISTS) {
        retQuery = database->query(FileIndexer::Statements::getFileInfoById, record->file_id);
        if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
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
    auto retQuery = database->query(FileIndexer::Statements::getFileInfoByPath, record->path.c_str());
    FileIndexer::FileRecord retRecord(retQuery.get());

    if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
        // file do not exist in db, insert a new file record
        return database->execute(FileIndexer::Statements::insertFileInfo,
                                 // retRecord.file_id,
                                 record->path.c_str(),
                                 record->size,
                                 record->mime_type,
                                 record->mtime,
                                 record->directory.c_str(),
                                 record->file_type);
    }
    // update existing file record
    return database->execute(FileIndexer::Statements::updateFileInfo,
                             record->path.c_str(),
                             record->size,
                             record->mime_type,
                             record->mtime,
                             record->directory.c_str(),
                             record->file_type,
                             retRecord.file_id);
}

auto FileIndexerAgent::dbDeleteFile(std::unique_ptr<FileIndexer::FileRecord> record) -> bool
{
    unsigned int file_id = FileIndexer::FILE_ID_NOT_EXISTS;
    if (false == record->path.empty()) {
        auto retQuery = database->query(FileIndexer::Statements::getFileIdByPath, record->path.c_str());
        if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
            return false;
        }
        else {
            file_id = (*retQuery)[0].getUInt32();
        }
    }
    else {
        file_id = record->file_id;
    }
    return database->execute(FileIndexer::Statements::deleteFileById, file_id, file_id);
}

auto FileIndexerAgent::dbDeleteAllFilesInDir(std::unique_ptr<FileIndexer::FileRecord> record) -> bool
{
    bool retStatus = false;
    if (false == record->directory.empty()) {
        auto retQuery = database->query(FileIndexer::Statements::getFilesIdByDir, record->directory.c_str());
        if (nullptr == retQuery || DatabaseAgent::ZERO_ROWS_FOUND == retQuery->getRowCount()) {
            return false;
        }
        else {
            do {
                unsigned int file_id = (*retQuery)[0].getUInt32();
                retStatus            = database->execute(FileIndexer::Statements::deleteFileById, file_id, file_id);
            } while (retQuery->nextRow() && retStatus);
            return retStatus;
        }
    }
    else {
        return false;
    }
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
    return sys::msgHandled();
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

            parentService->bus.sendUnicast(std::move(updateMsg), msg->sender);
            dbSetRecord(std::make_unique<FileIndexer::FileRecord>(record));

            for (auto recipient : fileChangeRecipents[record.directory]) {
                auto notifyMsg = std::make_shared<FileIndexer::Messages::DirectoryContentChangedMessage>(
                    std::make_unique<std::string>(record.directory));
                parentService->bus.sendUnicast(std::move(notifyMsg), recipient);
            }
        }
    }
    return sys::msgHandled();
}

auto FileIndexerAgent::handleDeleteFile(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::DeleteFileMessage *>(req)) {
        auto recordPtr                 = std::move(msg->record);
        FileIndexer::FileRecord record = *recordPtr;
        msg->dbRecord                  = dbGetRecord(std::make_unique<FileIndexer::FileRecord>(record));

        auto deleteMsg = std::make_shared<FileIndexer::Messages::FileDeletedMessage>(std::move(recordPtr));

        parentService->bus.sendUnicast(std::move(deleteMsg), msg->sender);
        dbDeleteFile(std::make_unique<FileIndexer::FileRecord>(record));

        for (auto recipient : fileChangeRecipents[record.directory]) {
            auto notifyMsg = std::make_shared<FileIndexer::Messages::DirectoryContentChangedMessage>(
                std::make_unique<std::string>(record.directory));
            parentService->bus.sendUnicast(std::move(notifyMsg), recipient);
        }
    }
    return sys::msgHandled();
}

auto FileIndexerAgent::handleDeleteAllFilesInDir(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::DeleteAllFilesInDirMessage *>(req)) {
        auto recordPtr                 = std::move(msg->record);
        FileIndexer::FileRecord record = *recordPtr;
        msg->dbRecord                  = dbGetRecord(std::make_unique<FileIndexer::FileRecord>(record));

        auto deleteMsg =
            std::make_shared<FileIndexer::Messages::AllFilesInDirDeletedDeletedMessage>(std::move(recordPtr));

        parentService->bus.sendUnicast(std::move(deleteMsg), msg->sender);
        dbDeleteAllFilesInDir(std::make_unique<FileIndexer::FileRecord>(record));

        for (auto recipient : fileChangeRecipents[record.directory]) {
            auto notifyMsg = std::make_shared<FileIndexer::Messages::DirectoryContentChangedMessage>(
                std::make_unique<std::string>(record.directory));
            parentService->bus.sendUnicast(std::move(notifyMsg), recipient);
        }
    }
    return sys::msgHandled();
}

auto FileIndexerAgent::dbGetProperty(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> FileIndexer::FileMetadata
{
    auto itr      = metaData->properties.begin();
    auto property = itr->first;

    std::unique_ptr<QueryResult> retQuery = nullptr;

    retQuery = database->query(FileIndexer::Statements::getPropertyValue, property.c_str(), metaData->path.c_str());
    if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
        return *metaData;
    }
    FileIndexer::FileMetadata retMetaData;
    retMetaData.file_id = (*retQuery)[0].getUInt32();
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
        retMetaData.file_id = (*retQuery)[0].getUInt32();
        retMetaData.properties.clear();
        do {
            retMetaData.properties.emplace((*retQuery)[1].getString(), (*retQuery)[2].getString());
        } while (retQuery->nextRow());
    }
    return retMetaData;
}

auto FileIndexerAgent::dbSetProperty(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool
{
    std::unique_ptr<QueryResult> retQuery = nullptr;
    unsigned int fileId;

    if (false == metaData->path.empty()) {
        retQuery = database->query(FileIndexer::Statements::getFileInfoByPath, metaData->path.c_str());
        if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
            return false;
        }
        fileId = (*retQuery)[0].getUInt32();
    }
    else if (metaData->file_id != FileIndexer::FILE_ID_NOT_EXISTS) {
        retQuery = database->query(FileIndexer::Statements::getFileInfoById, metaData->file_id);
        if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
            return false;
        }
        fileId = metaData->file_id;
    }
    else {
        // there is no way to identify the file (id or path)
        return false;
    }

    auto itr      = metaData->properties.begin();
    auto property = itr->first;
    auto value    = itr->second;
    return database->execute(FileIndexer::Statements::insertPropertyValue, fileId, property.c_str(), value.c_str());
}

auto FileIndexerAgent::dbSetProperties(std::unique_ptr<FileIndexer::FileMetadata> metaData) -> bool
{
    bool statusCode                       = true;
    std::unique_ptr<QueryResult> retQuery = nullptr;
    unsigned int fileId;

    if (false == metaData->path.empty()) {
        retQuery = database->query(FileIndexer::Statements::getFileInfoByPath, metaData->path.c_str());
        if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
            return false;
        }
        fileId = (*retQuery)[0].getUInt32();
    }
    else if (metaData->file_id != FileIndexer::FILE_ID_NOT_EXISTS) {
        retQuery = database->query(FileIndexer::Statements::getFileInfoById, metaData->file_id);
        if (nullptr == retQuery || DatabaseAgent::ONE_ROW_FOUND != retQuery->getRowCount()) {
            return false;
        }
        fileId = metaData->file_id;
    }
    else {
        // there is no way to identify the file (id or path)
        return false;
    }

    for (auto propVal : metaData->properties) {
        statusCode = database->execute(
            FileIndexer::Statements::insertPropertyValue, fileId, propVal.first.c_str(), propVal.second.c_str());
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
    return sys::msgHandled();
}

auto FileIndexerAgent::handleGetAllProperties(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::GetAllPropertiesMessage *>(req)) {
        auto metaDataPtr                   = std::move(msg->metaData);
        FileIndexer::FileMetadata metaData = *metaDataPtr;
        msg->dbMetaData                    = dbGetAllProperties(std::make_unique<FileIndexer::FileMetadata>(metaData));
        return std::make_shared<FileIndexer::Messages::GetPropertyResponseMessage>(
            std::make_unique<FileIndexer::FileMetadata>(msg->dbMetaData));
    }
    return sys::msgHandled();
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
            parentService->bus.sendUnicast(std::move(updateMsg), msg->sender);
            dbSetProperty(std::make_unique<FileIndexer::FileMetadata>(metaData));
        }
    }
    return sys::msgHandled();
}

auto FileIndexerAgent::handleSetProperties(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<FileIndexer::Messages::SetPropertiesMessage *>(req)) {
        auto metaDataPtr                   = std::move(msg->metaData);
        FileIndexer::FileMetadata metaData = *metaDataPtr;
        msg->dbMetaData                    = dbGetAllProperties(std::make_unique<FileIndexer::FileMetadata>(metaData));
        auto updateMsg                     = std::make_shared<FileIndexer::Messages::PropertyChangedMessage>(
            std::move(metaDataPtr), std::make_unique<FileIndexer::FileMetadata>(msg->dbMetaData));
        parentService->bus.sendUnicast(std::move(updateMsg), msg->sender);
        dbSetProperties(std::make_unique<FileIndexer::FileMetadata>(metaData));

        for (auto recipient : fileChangeRecipents[metaData.directory]) {
            auto notifyMsg = std::make_shared<FileIndexer::Messages::DirectoryContentChangedMessage>(
                std::make_unique<std::string>(metaData.directory));
            parentService->bus.sendUnicast(std::move(notifyMsg), recipient);
        }
    }
    return sys::msgHandled();
}
