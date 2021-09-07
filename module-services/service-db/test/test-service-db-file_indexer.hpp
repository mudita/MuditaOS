// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DatabaseAgent.hpp>
#include <service-db/FileIndexerMessages.hpp>
#include <service-db/agents/file_indexer/FileIndexerAgent.hpp>

class FileIndexerTest : public FileIndexerAgent
{
  public:
    FileIndexerTest(sys::Service *parentService) : FileIndexerAgent(parentService){};
    ~FileIndexerTest() = default;

    auto handleGetRecordTest(FileIndexer::Messages::GetRecordMessage *req) -> sys::MessagePointer
    {
        return FileIndexerAgent::handleGetRecord(req);
    }

    auto handleSetRecordTest(FileIndexer::Messages::SetRecordMessage *req) -> sys::MessagePointer
    {
        return FileIndexerAgent::handleSetRecord(req);
    }

    auto handleListDirTest(FileIndexer::Messages::GetListDirRequest *req) -> sys::MessagePointer
    {
        return FileIndexerAgent::handleListDir(req);
    }

    auto handleGetAllPropertiesTest(FileIndexer::Messages::GetAllPropertiesMessage *req) -> sys::MessagePointer
    {
        return FileIndexerAgent::handleGetAllProperties(req);
    }

    auto handleSetPropertiesTest(FileIndexer::Messages::SetPropertiesMessage *req) -> sys::MessagePointer
    {
        return FileIndexerAgent::handleSetProperties(req);
    }

    auto handleRegisterOnFileChangeTest(FileIndexer::Messages::RegisterOnFileChange *req) -> sys::MessagePointer
    {
        return FileIndexerAgent::handleRegisterOnFileChange(req);
    }

    auto handleUnregisterOnFileChangeTest(FileIndexer::Messages::UnregisterOnFileChange *req) -> sys::MessagePointer
    {
        return FileIndexerAgent::handleUnregisterOnFileChange(req);
    }

    auto handleDeleteFileTest(FileIndexer::Messages::DeleteFileMessage *req) -> sys::MessagePointer
    {
        return FileIndexerAgent::handleDeleteFile(req);
    }

    auto handleDeleteAllFilesInDirTest(FileIndexer::Messages::DeleteAllFilesInDirMessage *req) -> sys::MessagePointer
    {
        return FileIndexerAgent::handleDeleteAllFilesInDir(req);
    }

    unsigned int getNumberOfRecipients(std::string directory)
    {
        unsigned int recipientCount = 0;
        for (auto recipient : fileChangeRecipents[directory]) {
            LOG_INFO("Notifications:  directory= %s service= %s", directory.c_str(), recipient.c_str());
            recipientCount++;
        }
        return recipientCount++;
    }
};
