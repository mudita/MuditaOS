// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DBServiceName.hpp>
#include <service-db/ServiceDBCommon.hpp>

class AlarmEventRecordInterface;
class AlarmsRecordInterface;
class CalllogDB;
class CalllogRecordInterface;
class ContactRecordInterface;
class ContactsDB;
class CountryCodeRecordInterface;
class CountryCodesDB;
class DatabaseAgent;
class EventsDB;
class NotesDB;
class NotesRecordInterface;
class NotificationsDB;
class NotificationsRecordInterface;
class SMSRecordInterface;
class SMSTemplateRecordInterface;
class SettingsDB;

class SmsDB;
class ThreadRecordInterface;
namespace Quotes
{
    class QuotesAgent;
}

namespace db::multimedia_files
{
    class MultimediaFilesDB;
    class MultimediaFilesRecordInterface;
} // namespace db::multimedia_files

class ServiceDB : public ServiceDBCommon
{
  public:
    ~ServiceDB() override;

    bool StoreIntoBackup(const std::filesystem::path &backupPath);

  private:
    std::unique_ptr<EventsDB> eventsDB;
    std::unique_ptr<SmsDB> smsDB;
    std::unique_ptr<ContactsDB> contactsDB;
    std::unique_ptr<NotesDB> notesDB;
    std::unique_ptr<CalllogDB> calllogDB;
    std::unique_ptr<CountryCodesDB> countryCodesDB;
    std::unique_ptr<NotificationsDB> notificationsDB;
    std::unique_ptr<Database> predefinedQuotesDB;
    std::unique_ptr<Database> customQuotesDB;
    std::unique_ptr<db::multimedia_files::MultimediaFilesDB> multimediaFilesDB;

    std::unique_ptr<AlarmEventRecordInterface> alarmEventRecordInterface;
    std::unique_ptr<SMSRecordInterface> smsRecordInterface;
    std::unique_ptr<ThreadRecordInterface> threadRecordInterface;
    std::unique_ptr<SMSTemplateRecordInterface> smsTemplateRecordInterface;
    std::unique_ptr<ContactRecordInterface> contactRecordInterface;
    std::unique_ptr<NotesRecordInterface> notesRecordInterface;
    std::unique_ptr<CalllogRecordInterface> calllogRecordInterface;
    std::unique_ptr<CountryCodeRecordInterface> countryCodeRecordInterface;
    std::unique_ptr<NotificationsRecordInterface> notificationsRecordInterface;
    std::unique_ptr<Quotes::QuotesAgent> quotesRecordInterface;
    std::unique_ptr<db::multimedia_files::MultimediaFilesRecordInterface> multimediaFilesRecordInterface;

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
            manifest.dependencies = {service::name::file_indexer}; // OK
#endif
            manifest.timeout = std::chrono::minutes{1};
            return manifest;
        }
    };
} // namespace sys
