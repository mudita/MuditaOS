// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DBServiceName.hpp>
#include <service-db/ServiceDBCommon.hpp>

class AlarmEventRecordInterface;
class EventsDB;

class ThreadRecordInterface;

class ServiceDB : public ServiceDBCommon
{
  public:
    ~ServiceDB() override;

    bool StoreIntoBackup(const std::filesystem::path &backupPath);

  private:
    std::unique_ptr<EventsDB> eventsDB;
    std::unique_ptr<AlarmEventRecordInterface> alarmEventRecordInterface;

    db::Interface *getInterface(db::Interface::Name interface) override;
    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
    sys::ReturnCodes InitHandler() override;
};

namespace sys
{
    template <> struct ManifestTraits<ServiceDB>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name = service::name::db;
#if ENABLE_FILEINDEXER_SERVICE
            manifest.dependencies = {service::name::file_indexer.data()};
#endif
            manifest.timeout = std::chrono::minutes{1};
            return manifest;
        }
    };
} // namespace sys
