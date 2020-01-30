
/*
 * @file ServiceDB.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#ifndef PUREPHONE_SERVICEDB_HPP
#define PUREPHONE_SERVICEDB_HPP

#include "Interface/AlarmsRecord.hpp"
#include "Interface/CalllogRecord.hpp"
#include "Interface/ContactRecord.hpp"
#include "Interface/CountryCodeRecord.hpp"
#include "Interface/NotesRecord.hpp"
#include "Interface/SMSRecord.hpp"
#include "Interface/SettingsRecord.hpp"
#include "Interface/ThreadRecord.hpp"
#include "Service/Message.hpp"
#include "Service/Service.hpp"

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

    std::unique_ptr<SettingsRecordInterface> settingsRecordInterface;
    std::unique_ptr<SMSRecordInterface> smsRecordInterface;
    std::unique_ptr<ThreadRecordInterface> threadRecordInterface;
    std::unique_ptr<ContactRecordInterface> contactRecordInterface;
    std::unique_ptr<AlarmsRecordInterface> alarmsRecordInterface;
    std::unique_ptr<NotesRecordInterface> notesRecordInterface;
    std::unique_ptr<CalllogRecordInterface> calllogRecordInterface;
    std::unique_ptr<CountryCodeRecordInterface> countryCodeRecordInterface;

  protected:
  public:
    ServiceDB();
    virtual ~ServiceDB();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;
    static const char *serviceName;
};

#endif // PUREPHONE_SERVICEDB_HPP
