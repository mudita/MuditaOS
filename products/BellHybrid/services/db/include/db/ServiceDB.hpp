// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <QuotesAgent.hpp>
#include <service-db/DBServiceName.hpp>
#include <service-db/ServiceDBCommon.hpp>
#include <service-db/ServiceDBDependencies.hpp>

class AlarmEventRecordInterface;
class EventsDB;

class ThreadRecordInterface;

namespace db::multimedia_files
{
    class MultimediaFilesDB;
    class MultimediaFilesRecordInterface;
} // namespace db::multimedia_files

class ServiceDB : public ServiceDBCommon
{
  public:
    ~ServiceDB() override;

  private:
    std::unique_ptr<EventsDB> eventsDB;
    std::unique_ptr<Database> quotesDB;
    std::unique_ptr<db::multimedia_files::MultimediaFilesDB> multimediaFilesDB;

    std::unique_ptr<Quotes::QuotesAgent> quotesRecordInterface;
    std::unique_ptr<AlarmEventRecordInterface> alarmEventRecordInterface;
    std::unique_ptr<db::multimedia_files::MultimediaFilesRecordInterface> multimediaFilesRecordInterface;

    db::Interface *getInterface(db::Interface::Name interface) override;
    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
    sys::ReturnCodes InitHandler() override;

    bool StoreIntoSyncPackage(const std::filesystem::path &syncPackagePath);
};

namespace sys
{
    template <>
    struct ManifestTraits<ServiceDB>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::db;
            manifest.dependencies = sys::dependencies::getDependenciesFor<ServiceDB>();
            manifest.timeout      = std::chrono::minutes{1};
            return manifest;
        }
    };
} // namespace sys
