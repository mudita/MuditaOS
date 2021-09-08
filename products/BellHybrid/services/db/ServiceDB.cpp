// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <db/ServiceDB.hpp>

#include <module-db/Databases/EventsDB.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>

#include <service-db/DBServiceMessage.hpp>
#include <service-db/agents/settings/SettingsAgent.hpp>
#include <service-db/agents/file_indexer/FileIndexerAgent.hpp>
#include <time/ScopedTime.hpp>

#include <purefs/filesystem_paths.hpp>

ServiceDB::~ServiceDB()
{
    eventsDB.reset();

    Database::deinitialize();
    LOG_INFO("[ServiceDB] Cleaning resources");
}

db::Interface *ServiceDB::getInterface(db::Interface::Name interface)
{
    switch (interface) {
    case db::Interface::Name::AlarmEvents:
        return alarmEventRecordInterface.get();
    default:
        LOG_INFO("Not supported interface");
    }

    return nullptr;
}

sys::MessagePointer ServiceDB::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    auto responseMsg = std::static_pointer_cast<sys::ResponseMessage>(ServiceDBCommon::DataReceivedHandler(msgl, resp));
    if (responseMsg) {
        return responseMsg;
    }
    auto type = static_cast<MessageType>(msgl->messageType);
    switch (type) {

    case MessageType::DBServiceBackup: {
        auto time   = utils::time::Scoped("DBServiceBackup");
        auto msg    = static_cast<DBServiceMessageBackup *>(msgl);
        auto ret    = StoreIntoBackup({msg->backupPath});
        responseMsg = std::make_shared<DBServiceResponseMessage>(ret);
    } break;

    default:
        break;
    }

    if (responseMsg == nullptr) {
        return std::make_shared<sys::ResponseMessage>();
    }

    responseMsg->responseTo = msgl->messageType;
    return responseMsg;
}

sys::ReturnCodes ServiceDB::InitHandler()
{
    if (const auto returnCode = ServiceDBCommon::InitHandler(); returnCode != sys::ReturnCodes::Success) {
        return returnCode;
    }

    // Create databases
    eventsDB = std::make_unique<EventsDB>((purefs::dir::getUserDiskPath() / "events.db").c_str());

    // Create record interfaces
    alarmEventRecordInterface = std::make_unique<AlarmEventRecordInterface>(eventsDB.get());

    databaseAgents.emplace(std::make_unique<SettingsAgent>(this, "settings_bell.db"));

    for (auto &dbAgent : databaseAgents) {
        dbAgent->initDb();
        dbAgent->registerMessages();
    }

    return sys::ReturnCodes::Success;
}

bool ServiceDB::StoreIntoBackup(const std::filesystem::path &backupPath)
{
    if (eventsDB->storeIntoFile(backupPath / std::filesystem::path(eventsDB->getName()).filename()) == false) {
        LOG_ERROR("eventsDB backup failed");
        return false;
    }

    for (auto &db : databaseAgents) {
        if (db.get() && db.get()->getAgentName() == "settingsAgent") {

            if (db->storeIntoFile(backupPath / std::filesystem::path(db->getDbFilePath()).filename()) == false) {
                LOG_ERROR("settingsAgent backup failed");
                return false;
            }
            break;
        }
    }

    return true;
}
