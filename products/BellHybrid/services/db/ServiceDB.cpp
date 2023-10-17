// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <db/ServiceDB.hpp>
#include <db/BellFactorySettings.hpp>

#include "agents/MeditationStatsAgent.hpp"

#include <module-db/databases/EventsDB.hpp>
#include <module-db/databases/MultimediaFilesDB.hpp>

#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>

#include <service-db/DBServiceMessage.hpp>
#include <service-db/agents/settings/SettingsAgent.hpp>
#include <service-db/Settings.hpp>
#include <time/ScopedTime.hpp>

#include <purefs/filesystem_paths.hpp>
#include <CrashdumpMetadataStore.hpp>
#include <product/version.hpp>

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

    const auto factorySettings = std::make_unique<settings::BellFactorySettings>();
    databaseAgents.emplace(std::make_unique<SettingsAgent>(this, "settings_bell.db", factorySettings.get()));
    databaseAgents.emplace(std::make_unique<service::db::agents::MeditationStats>(this, "meditation_stats.db"));

    for (auto &dbAgent : databaseAgents) {
        dbAgent->registerMessages();
    }

    const auto settings = std::make_unique<settings::Settings>();
    settings->init(service::ServiceProxy(shared_from_this()));

    /* Save metadata for crashdump generation purpose */
    const auto serialNumberPath =
        settings::factory::entry_key + std::string("/") + settings::factory::serial_number_key;
    Store::CrashdumpMetadata::getInstance().setSerialNumber(
        settings->getValue(serialNumberPath, settings::SettingsScope::Global));
    Store::CrashdumpMetadata::getInstance().setCommitHash(GIT_REV);
    Store::CrashdumpMetadata::getInstance().setOsVersion(VERSION);
    Store::CrashdumpMetadata::getInstance().setProductName("BellHybrid");

    return sys::ReturnCodes::Success;
}
