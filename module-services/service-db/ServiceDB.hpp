#pragma once

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
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "messages/DBNotificationMessage.hpp"

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

  public:
    ServiceDB();
    ~ServiceDB() override;

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) final;

    bool StoreIntoBackup(const std::string &backupPath);
    void sendUpdateNotification(db::Interface::Name interface, db::Query::Type type);
};

