// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <Service/Service.hpp>
#include <functional>
#include <thread> // for Message_t, ResponseMessage, DataMessage, Message
#include <service-db/ServiceDBCommon.hpp>

#include <service-db/test/test-service-db-file_indexer.hpp>
#include <service-db/FileIndexerMessages.hpp>

class ClientService : public sys::Service
{
  public:
    using sys::Service::Service;

    explicit ClientService(std::string name) : Service(name)
    {}

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::ReturnCodes InitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        return sys::ReturnCodes::Success;
    }
};

TEST_CASE("FileIndexer")
{
    Database::initialize();
    std::string serviceName = "file_indexer_client";
    ClientService file_indexer_client_service(serviceName);
    file_indexer_client_service.InitHandler();

    SECTION("init db, register /unregister on file change")
    {
        std::cout << "Init File Indexer DB" << std::endl << std::flush;
        std::unique_ptr<FileIndexerTest> fiAgentTest{new FileIndexerTest(&file_indexer_client_service)};
        fiAgentTest->initDb();
        fiAgentTest->registerMessages();

        std::cout << "Register on file change" << std::endl << std::flush;
        std::string inputDirReg = "mp3";
        auto recordPtr          = std::make_unique<std::string>(inputDirReg);
        auto regMsg             = std::make_shared<FileIndexer::Messages::RegisterOnFileChange>(std::move(recordPtr));
        regMsg->sender          = serviceName;
        sys::MessagePointer recMsg = fiAgentTest->handleRegisterOnFileChangeTest(regMsg.get());
        fiAgentTest->getNumberOfRecipients(inputDirReg);

        regMsg->sender        = "serv1";
        recMsg                = fiAgentTest->handleRegisterOnFileChangeTest(regMsg.get());
        auto recipientsCount1 = fiAgentTest->getNumberOfRecipients(inputDirReg);

        std::cout << "Unregister on file change" << std::endl << std::flush;
        std::string inputDirUnReg = "mp3";
        auto strPtr               = std::make_unique<std::string>(inputDirUnReg);
        auto unregMsg             = std::make_shared<FileIndexer::Messages::UnregisterOnFileChange>(std::move(strPtr));
        unregMsg->sender          = serviceName;
        recMsg                    = fiAgentTest->handleUnregisterOnFileChangeTest(unregMsg.get());
        auto recipientsCount2     = fiAgentTest->getNumberOfRecipients(inputDirUnReg);

        REQUIRE(recipientsCount2 == recipientsCount1 - 1);
        REQUIRE(fiAgentTest->getAgentName() == "fileIndexerAgent");
    }

    SECTION("SetRecord & GetRecord")
    {
        FileIndexer::FileRecord record{};
        std::cout << "Test File indexer Get/Set/Get Record sequence:" << std::endl << std::flush;

        // GetRecord
        record.file_id = 1;
        auto recordPtr = std::make_unique<FileIndexer::FileRecord>(record);
        auto getMsg    = std::make_shared<FileIndexer::Messages::GetRecordMessage>(std::move(recordPtr));
        std::unique_ptr<FileIndexerTest> fiAgentTest{new FileIndexerTest(&file_indexer_client_service)};
        sys::MessagePointer recMsg = fiAgentTest->handleGetRecordTest(getMsg.get());
        auto msg                   = dynamic_cast<FileIndexer::Messages::GetRecordResponseMessage *>(recMsg.get());
        auto fileRec               = msg->record.get();

        LOG_INFO("file id= %d file path= %s  size = %d  mime= %d  mtime = %d directory= %s ftype= %d",
                 fileRec->file_id,
                 fileRec->path.c_str(),
                 fileRec->size,
                 fileRec->mime_type,
                 fileRec->mtime,
                 fileRec->directory.c_str(),
                 fileRec->file_type);

        REQUIRE(fileRec->file_id == record.file_id);

        record = *fileRec; // fill record by data recived from db

        // SetRecord - modify mime_type
        record.mime_type = 2;
        recordPtr        = std::make_unique<FileIndexer::FileRecord>(record);
        auto setMsg      = std::make_shared<FileIndexer::Messages::SetRecordMessage>(std::move(recordPtr));
        setMsg->sender   = serviceName;
        recMsg           = fiAgentTest->handleSetRecordTest(setMsg.get());

        // GetRecord to confirm changes in mime_type
        recordPtr = std::make_unique<FileIndexer::FileRecord>(record);
        getMsg    = std::make_shared<FileIndexer::Messages::GetRecordMessage>(std::move(recordPtr));
        recMsg    = fiAgentTest->handleGetRecordTest(getMsg.get());
        msg       = dynamic_cast<FileIndexer::Messages::GetRecordResponseMessage *>(recMsg.get());
        fileRec   = msg->record.get();

        LOG_INFO("file id= %d file path= %s  size = %d  mime= %d  mtime = %d directory= %s ftype= %d",
                 fileRec->file_id,
                 fileRec->path.c_str(),
                 fileRec->size,
                 fileRec->mime_type,
                 fileRec->mtime,
                 fileRec->directory.c_str(),
                 fileRec->file_type);
        // check if new value was set
        REQUIRE(fileRec->mime_type == record.mime_type);
        // check if the rest of the record did not change
        REQUIRE(fileRec->file_id == record.file_id);
        REQUIRE(fileRec->path == record.path);
        REQUIRE(fileRec->size == record.size);
        REQUIRE(fileRec->mtime == record.mtime);
        REQUIRE(fileRec->directory == record.directory);
        REQUIRE(fileRec->file_type == record.file_type);
    }

    SECTION("List dir - support for listview")
    {
        FileIndexer::ListDir inputData{};
        std::cout << "List dir - support for listview" << std::endl << std::flush;

        // GetListDir
        inputData.directory   = "mp3";
        inputData.offset      = 1;
        inputData.limit       = 2;

        auto recordPtr = std::make_unique<FileIndexer::ListDir>(inputData);
        auto getMsg    = std::make_shared<FileIndexer::Messages::GetListDirRequest>(std::move(recordPtr));
        std::unique_ptr<FileIndexerTest> fiAgentTest{new FileIndexerTest(&file_indexer_client_service)};
        sys::MessagePointer recMsg = fiAgentTest->handleListDirTest(getMsg.get());
        auto msg                   = dynamic_cast<FileIndexer::Messages::GetListDirResponse *>(recMsg.get());
        auto fileCount             = msg->getCount();
        auto fileList              = msg->getResults();
        LOG_INFO("Total number of files in %s = %d", inputData.directory.c_str(), fileCount);
        LOG_INFO("Limit= %d Offset= %d", inputData.limit, inputData.offset);
        for (auto fileRec : fileList) {
            LOG_INFO("file id= %d file path= %s  size = %d  mime= %d  mtime = %d directory= %s ftype= %d",
                     fileRec.file_id,
                     fileRec.path.c_str(),
                     fileRec.size,
                     fileRec.mime_type,
                     fileRec.mtime,
                     fileRec.directory.c_str(),
                     fileRec.file_type);
        }
        REQUIRE(fileList.size() == inputData.limit);
        REQUIRE(fileList[0].directory == inputData.directory);
    }

    SECTION("get/set Properties")
    {
        FileIndexer::FileMetadata inputMetaData{};
        std::cout << "get/set Properties" << std::endl << std::flush;

        // Get Properties
        inputMetaData.path = "mp3/track3.mp3";
        LOG_INFO("File path= %s", inputMetaData.path.c_str());

        auto recordPtr = std::make_unique<FileIndexer::FileMetadata>(inputMetaData);
        auto getMsg    = std::make_shared<FileIndexer::Messages::GetAllPropertiesMessage>(std::move(recordPtr));
        std::unique_ptr<FileIndexerTest> fiAgentTest{new FileIndexerTest(&file_indexer_client_service)};
        sys::MessagePointer recMsg = fiAgentTest->handleGetAllPropertiesTest(getMsg.get());
        auto msg                   = dynamic_cast<FileIndexer::Messages::GetPropertyResponseMessage *>(recMsg.get());
        for (auto prop : msg->metaData->properties) {
            LOG_INFO("file id= %d property= %s  value = %s",
                     msg->metaData->file_id,
                     prop.first.c_str(),
                     prop.second.c_str());
        }
        REQUIRE(msg->metaData->properties.size() > 0);

        // set Properties
        inputMetaData.properties.clear();
        inputMetaData.properties.emplace("year", "2015");

        std::string newProperty      = "time";
        std::string newPropertyValue = "5min";
        inputMetaData.properties.emplace(newProperty, newPropertyValue);
        recordPtr      = std::make_unique<FileIndexer::FileMetadata>(inputMetaData);
        auto setMsg    = std::make_shared<FileIndexer::Messages::SetPropertiesMessage>(std::move(recordPtr));
        setMsg->sender = serviceName;
        recMsg         = fiAgentTest->handleSetPropertiesTest(setMsg.get());

        // Get Properties
        recordPtr = std::make_unique<FileIndexer::FileMetadata>(inputMetaData);
        getMsg    = std::make_shared<FileIndexer::Messages::GetAllPropertiesMessage>(std::move(recordPtr));
        recMsg    = fiAgentTest->handleGetAllPropertiesTest(getMsg.get());
        msg       = dynamic_cast<FileIndexer::Messages::GetPropertyResponseMessage *>(recMsg.get());
        for (auto prop : msg->metaData->properties) {
            LOG_INFO("file id= %d property= %s  value = %s",
                     msg->metaData->file_id,
                     prop.first.c_str(),
                     prop.second.c_str());
        }
        //  serch if a new property was set correctly
        std::string newPropertyValueFound;
        auto it = msg->metaData->properties.find(newProperty);
        if (msg->metaData->properties.end() != it) {
            newPropertyValueFound = it->second;
        }
        REQUIRE(newPropertyValueFound == newPropertyValue);
    }

    SECTION("DeleteFile /DeleteAllFilesInDir")
    {
        FileIndexer::ListDir inputData{};
        FileIndexer::FileRecord record{};
        std::cout << "DeleteFile /DeleteAllFilesInDir" << std::endl << std::flush;

        // SetRecord - add files
        auto recordPtr = std::make_unique<FileIndexer::FileRecord>(record);
        std::unique_ptr<FileIndexerTest> fiAgentTest{new FileIndexerTest(&file_indexer_client_service)};
        record.directory = "wav";
        record.path      = "wav/file1.wav";

        recordPtr                  = std::make_unique<FileIndexer::FileRecord>(record);
        auto setMsg                = std::make_shared<FileIndexer::Messages::SetRecordMessage>(std::move(recordPtr));
        setMsg->sender             = serviceName;
        sys::MessagePointer recMsg = fiAgentTest->handleSetRecordTest(setMsg.get());

        record.path    = "wav/file2.wav";
        recordPtr      = std::make_unique<FileIndexer::FileRecord>(record);
        setMsg         = std::make_shared<FileIndexer::Messages::SetRecordMessage>(std::move(recordPtr));
        setMsg->sender = serviceName;
        recMsg         = fiAgentTest->handleSetRecordTest(setMsg.get());

        record.path    = "wav/file3.wav";
        recordPtr      = std::make_unique<FileIndexer::FileRecord>(record);
        setMsg         = std::make_shared<FileIndexer::Messages::SetRecordMessage>(std::move(recordPtr));
        setMsg->sender = serviceName;
        recMsg         = fiAgentTest->handleSetRecordTest(setMsg.get());

        std::cout << "Get listDir" << std::endl << std::flush;
        inputData.directory   = "wav";
        inputData.offset      = 0;
        inputData.limit       = 10;
        auto inputDataPtr     = std::make_unique<FileIndexer::ListDir>(inputData);
        auto getListDirMsg    = std::make_shared<FileIndexer::Messages::GetListDirRequest>(std::move(inputDataPtr));
        recMsg                = fiAgentTest->handleListDirTest(getListDirMsg.get());
        auto msg              = dynamic_cast<FileIndexer::Messages::GetListDirResponse *>(recMsg.get());
        auto fileCount        = msg->getCount();
        auto fileList         = msg->getResults();
        LOG_INFO("Total number of files in %s = %d", inputData.directory.c_str(), fileCount);
        LOG_INFO("Limit= %d Offset= %d", inputData.limit, inputData.offset);
        for (auto fileRec : fileList) {
            LOG_INFO("file id= %d file path= %s  size = %d  mime= %d  mtime = %d directory= %s ftype= %d",
                     fileRec.file_id,
                     fileRec.path.c_str(),
                     fileRec.size,
                     fileRec.mime_type,
                     fileRec.mtime,
                     fileRec.directory.c_str(),
                     fileRec.file_type);
        }

        std::cout << "DeleteFile" << std::endl << std::flush;
        record.path      = "wav/file1.wav";
        record.directory = "wav";
        recordPtr        = std::make_unique<FileIndexer::FileRecord>(record);
        auto delMsg      = std::make_shared<FileIndexer::Messages::DeleteFileMessage>(std::move(recordPtr));
        delMsg->sender   = serviceName;
        auto delRecMsg   = fiAgentTest->handleDeleteFileTest(delMsg.get());

        inputData.directory    = "wav";
        inputData.offset       = 0;
        inputData.limit        = 10;
        inputDataPtr           = std::make_unique<FileIndexer::ListDir>(inputData);
        getListDirMsg          = std::make_shared<FileIndexer::Messages::GetListDirRequest>(std::move(inputDataPtr));
        recMsg                 = fiAgentTest->handleListDirTest(getListDirMsg.get());
        msg                    = dynamic_cast<FileIndexer::Messages::GetListDirResponse *>(recMsg.get());
        auto fileCountAfterDel = msg->getCount();
        REQUIRE(fileCountAfterDel == fileCount - 1);

        std::cout << "DeleteAllFilesInDir" << std::endl << std::flush;
        record.directory  = "wav";
        recordPtr         = std::make_unique<FileIndexer::FileRecord>(record);
        auto delAllMsg    = std::make_shared<FileIndexer::Messages::DeleteAllFilesInDirMessage>(std::move(recordPtr));
        delAllMsg->sender = serviceName;
        delRecMsg         = fiAgentTest->handleDeleteAllFilesInDirTest(delAllMsg.get());

        inputData.directory   = "wav";
        inputData.offset      = 0;
        inputData.limit       = 10;
        inputDataPtr          = std::make_unique<FileIndexer::ListDir>(inputData);
        getListDirMsg         = std::make_shared<FileIndexer::Messages::GetListDirRequest>(std::move(inputDataPtr));
        recMsg                = fiAgentTest->handleListDirTest(getListDirMsg.get());
        msg                   = dynamic_cast<FileIndexer::Messages::GetListDirResponse *>(recMsg.get());
        fileCountAfterDel     = msg->getCount();
        REQUIRE(fileCountAfterDel == 0);
        std::cout << "DeleteFile /DeleteAllFilesInDir  finished" << std::endl << std::flush;
    }

    Database::deinitialize();
}
