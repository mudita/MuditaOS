// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory> // for unique_ptr
#include <set>    // for set
#include <string> // for string

#include "Interface/AlarmsRecord.hpp"
#include "Interface/CalllogRecord.hpp"
#include "Interface/ContactRecord.hpp"
#include "Interface/CountryCodeRecord.hpp"
#include "Interface/NotesRecord.hpp"
#include "Interface/SMSRecord.hpp"
#include "Interface/SettingsRecord.hpp"
#include "Interface/ThreadRecord.hpp"
#include "Interface/SMSTemplateRecord.hpp"
#include "Interface/NotificationsRecord.hpp"
#include "Interface/EventsRecord.hpp"
#include "Interface/SettingsRecord_v2.hpp"
#include "Service/Message.hpp" // for Message_t, DataMessage (ptr only), ResponseMessage (ptr only)
#include "Service/Service.hpp" // for Service
#include "messages/DBNotificationMessage.hpp"
#include "agents/DatabaseAgent.hpp"
#include "agents/settings/SettingsAgent.hpp"
#include "BaseInterface.hpp"  // for Interface, Interface::Name
#include "Common/Query.hpp"   // for Query, Query::Type
#include "Service/Common.hpp" // for ReturnCodes, ServicePowerMode

class AlarmsDB;
class AlarmsRecordInterface;
class CalllogDB;
class CalllogRecordInterface;
class ContactRecordInterface;
class ContactsDB;
class CountryCodeRecordInterface;
class CountryCodesDB;
class DatabaseAgent;
class EventsDB;
class EventsRecordInterface;
class NotesDB;
class NotesRecordInterface;
class NotificationsDB;
class NotificationsRecordInterface;
class SMSRecordInterface;
class SMSTemplateRecordInterface;
class SettingsDB;
class SettingsRecordInterface;
class SettingsRecordInterface_v2;
class SmsDB;
class ThreadRecordInterface;

class ServiceDB : public sys::Service
{
  private:
    std::unique_ptr<SettingsDB> settingsDB;
    std::unique_ptr<SmsDB> smsDB;
    std::unique_ptr<ContactsDB> contactsDB;
    std::unique_ptr<AlarmsDB> alarmsDB;
    std::unique_ptr<NotesDB> notesDB;
    std::unique_ptr<CalllogDB> calllogDB;
    std::unique_ptr<CountryCodesDB> countryCodesDB;
    std::unique_ptr<NotificationsDB> notificationsDB;
    std::unique_ptr<EventsDB> eventsDB;

    std::unique_ptr<SettingsRecordInterface> settingsRecordInterface;
    std::unique_ptr<SMSRecordInterface> smsRecordInterface;
    std::unique_ptr<ThreadRecordInterface> threadRecordInterface;
    std::unique_ptr<SMSTemplateRecordInterface> smsTemplateRecordInterface;
    std::unique_ptr<ContactRecordInterface> contactRecordInterface;
    std::unique_ptr<AlarmsRecordInterface> alarmsRecordInterface;
    std::unique_ptr<NotesRecordInterface> notesRecordInterface;
    std::unique_ptr<CalllogRecordInterface> calllogRecordInterface;
    std::unique_ptr<CountryCodeRecordInterface> countryCodeRecordInterface;
    std::unique_ptr<NotificationsRecordInterface> notificationsRecordInterface;
    std::unique_ptr<EventsRecordInterface> eventsRecordInterface;
    std::unique_ptr<SettingsRecordInterface_v2> settingsRecordInterface_v2;

  protected:
    db::Interface *getInterface(db::Interface::Name interface);
    std::set<std::unique_ptr<DatabaseAgent>> databaseAgents;

  public:
    ServiceDB();
    ~ServiceDB() override;

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) final;

    bool StoreIntoBackup(const std::string &backupPath);
    void sendUpdateNotification(db::Interface::Name interface, db::Query::Type type);
};
