// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DBServiceName.hpp>
#include <service-db/ServiceDBCommon.hpp>

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
    std::unique_ptr<db::multimedia_files::MultimediaFilesDB> multimediaFilesDB;

    std::unique_ptr<AlarmEventRecordInterface> alarmEventRecordInterface;
    std::unique_ptr<db::multimedia_files::MultimediaFilesRecordInterface> multimediaFilesRecordInterface;

    db::Interface *getInterface(db::Interface::Name interface) override;
    sys::ReturnCodes InitHandler() override;
};

namespace sys
{
    template <>
    struct ManifestTraits<ServiceDB>
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
