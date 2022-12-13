// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <db/ServiceDB.hpp>

#include "agents/MeditationStatsAgent.hpp"

#include <module-db/databases/EventsDB.hpp>
#include <module-db/databases/MultimediaFilesDB.hpp>

#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>

#include <service-db/DBServiceMessage.hpp>
#include <service-db/agents/settings/SettingsAgent.hpp>
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
    case db::Interface::Name::MultimediaFiles:
        return multimediaFilesRecordInterface.get();
    default:
        LOG_INFO("Not supported interface");
    }

    return nullptr;
}

sys::ReturnCodes ServiceDB::InitHandler()
{
    if (const auto returnCode = ServiceDBCommon::InitHandler(); returnCode != sys::ReturnCodes::Success) {
        return returnCode;
    }

    // Create databases
    eventsDB          = std::make_unique<EventsDB>((purefs::dir::getDatabasesPath() / "events.db").c_str());
    multimediaFilesDB = std::make_unique<db::multimedia_files::MultimediaFilesDB>(
        (purefs::dir::getDatabasesPath() / "multimedia.db").c_str());

    // Create record interfaces
    alarmEventRecordInterface = std::make_unique<AlarmEventRecordInterface>(eventsDB.get());
    multimediaFilesRecordInterface =
        std::make_unique<db::multimedia_files::MultimediaFilesRecordInterface>(multimediaFilesDB.get());

    databaseAgents.emplace(std::make_unique<SettingsAgent>(this, "settings_bell.db"));
    databaseAgents.emplace(std::make_unique<service::db::agents::MeditationStats>(this, "meditation_stats.db"));

    for (auto &dbAgent : databaseAgents) {
        dbAgent->registerMessages();
    }

    return sys::ReturnCodes::Success;
}