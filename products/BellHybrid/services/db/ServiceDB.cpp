// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <db/ServiceDB.hpp>

#include "agents/MeditationStatsAgent.hpp"

#include <module-db/Databases/EventsDB.hpp>
#include <module-db/Databases/MultimediaFilesDB.hpp>

#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>

#include <service-db/DBServiceMessage.hpp>
#include <service-db/agents/settings/SettingsAgent.hpp>
#include <time/ScopedTime.hpp>

#include <purefs/filesystem_paths.hpp>

#include <serial-number-reader/SerialNumberReader.hpp>
#include <crashdump-serial-number/crashdump_serial_number.hpp>

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
    case db::Interface::Name::MultimediaFiles:
        return multimediaFilesRecordInterface.get();
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
    eventsDB          = std::make_unique<EventsDB>((purefs::dir::getUserDiskPath() / "events.db").c_str());
    multimediaFilesDB = std::make_unique<db::multimedia_files::MultimediaFilesDB>(
        (purefs::dir::getUserDiskPath() / "multimedia.db").c_str());

    // Create record interfaces
    alarmEventRecordInterface = std::make_unique<AlarmEventRecordInterface>(eventsDB.get());
    multimediaFilesRecordInterface =
        std::make_unique<db::multimedia_files::MultimediaFilesRecordInterface>(multimediaFilesDB.get());

    databaseAgents.emplace(std::make_unique<SettingsAgent>(this, "settings_bell.db"));
    databaseAgents.emplace(std::make_unique<service::db::agents::MeditationStats>(this, "meditation_stats.db"));

    for (auto &dbAgent : databaseAgents) {
        dbAgent->registerMessages();
    }

    LOG_INFO("Serial number: %s", serial_number_reader::readSerialNumber().c_str());

    // Saving serial number for crashdump generation purpose.
    crashdump::setSerialNumber(serial_number_reader::readSerialNumber());

    return sys::ReturnCodes::Success;
}

bool ServiceDB::StoreIntoBackup(const std::filesystem::path &backupPath)
{
    if (eventsDB->storeIntoFile(backupPath / std::filesystem::path(eventsDB->getName()).filename()) == false) {
        LOG_ERROR("eventsDB backup failed");
        return false;
    }

    return std::all_of(databaseAgents.cbegin(), databaseAgents.cend(), [&backupPath](const auto &agent) {
        if (not agent->storeIntoFile(backupPath / std::filesystem::path(agent->getDbFilePath()).filename())) {
            LOG_ERROR("%s backup failed", agent->getAgentName().c_str());
            return false;
        }
        return true;
    });
}
