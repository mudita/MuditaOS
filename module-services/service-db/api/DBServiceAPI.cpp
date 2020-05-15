#include "DBServiceAPI.hpp"

#include <messages/DBThreadMessage.hpp>
#include <messages/DBSettingsMessage.hpp>
#include <messages/DBSMSMessage.hpp>
#include <messages/DBSMSTemplateMessage.hpp>
#include <messages/DBContactMessage.hpp>
#include <messages/DBAlarmMessage.hpp>
#include <messages/DBNotesMessage.hpp>
#include <messages/DBCalllogMessage.hpp>
#include <messages/DBCountryCodeMessage.hpp>

#include <ServiceDB.hpp>
#include <includes/DBServiceName.hpp>

#include <Utils.hpp>

#include <cassert>

SettingsRecord DBServiceAPI::SettingsGet(sys::Service *serv)
{

    std::shared_ptr<DBSettingsMessage> msg = std::make_shared<DBSettingsMessage>(MessageType::DBSettingsGet);

    auto ret = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto respMsg                                = ret.second;
        DBSettingsResponseMessage *settingsResponse = reinterpret_cast<DBSettingsResponseMessage *>(respMsg.get());

        if (settingsResponse->retCode == true) {
            return settingsResponse->record;
        }
        else {
            return SettingsRecord{};
        }
    }
    else {
        return SettingsRecord{};
    }
}

bool DBServiceAPI::SettingsUpdate(sys::Service *serv, const SettingsRecord &rec)
{
    std::shared_ptr<DBSettingsMessage> msg = std::make_shared<DBSettingsMessage>(MessageType::DBSettingsUpdate, rec);

    auto ret = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    if (ret.first == sys::ReturnCodes::Success) {
        return true;
    }
    else {
        return false;
    }
}

uint32_t DBServiceAPI::SMSAdd(sys::Service *serv, const SMSRecord &rec)
{
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSAdd, rec);

    auto ret                          = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBSMSResponseMessage *smsResponse = reinterpret_cast<DBSMSResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)) {
        return (*smsResponse->records)[0].ID;
    }
    else {
        return 0;
    }
}

bool DBServiceAPI::SMSRemove(sys::Service *serv, uint32_t id)
{
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSRemove);
    msg->id                           = id;

    auto ret                          = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBSMSResponseMessage *smsResponse = reinterpret_cast<DBSMSResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::SMSUpdate(sys::Service *serv, const SMSRecord &rec)
{
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSUpdate, rec);

    auto ret                          = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBSMSResponseMessage *smsResponse = reinterpret_cast<DBSMSResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

SMSRecord DBServiceAPI::SMSGetLastRecord(sys::Service *serv)
{
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSGetLastRecord);

    auto ret                          = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBSMSResponseMessage *smsResponse = reinterpret_cast<DBSMSResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)) {
        return (*smsResponse->records)[0];
    }
    else {
        SMSRecord rec;
        rec.ID = 0;
        return rec;
    }
}

std::unique_ptr<std::vector<SMSRecord>> DBServiceAPI::SMSGetLimitOffset(sys::Service *serv,
                                                                        uint32_t offset,
                                                                        uint32_t limit)
{
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSGetSMSLimitOffset);
    msg->offset                       = offset;
    msg->limit                        = limit;

    auto ret                          = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBSMSResponseMessage *smsResponse = reinterpret_cast<DBSMSResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)) {
        return std::move(smsResponse->records);
    }
    else {
        return std::make_unique<std::vector<SMSRecord>>();
    }
}

std::unique_ptr<std::vector<SMSRecord>> DBServiceAPI::SMSGetLimitOffsetByThreadID(sys::Service *serv,
                                                                                  uint32_t offset,
                                                                                  uint32_t limit,
                                                                                  uint32_t id)
{
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSGetSMSLimitOffsetByThreadID);
    msg->offset                       = offset;
    msg->limit                        = limit;
    msg->id                           = id;

    auto ret                          = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBSMSResponseMessage *smsResponse = reinterpret_cast<DBSMSResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)) {
        return std::move(smsResponse->records);
    }
    else {
        return std::make_unique<std::vector<SMSRecord>>();
    }
}

uint32_t DBServiceAPI::SMSGetCount(sys::Service *serv, EntryState state)
{
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSGetCount>(state);

    auto ret  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    auto *sms = reinterpret_cast<DBSMSResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (sms->retCode == true)) {
        return sms->count;
    }
    else {
        return 0;
    }
}

std::unique_ptr<ThreadRecord> DBServiceAPI::ThreadGet(sys::Service *serv, uint32_t id)
{
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadGet);
    msg->id                              = id;

    auto ret                                = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBThreadResponseMessage *threadResponse = reinterpret_cast<DBThreadResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)) {
        return std::make_unique<ThreadRecord>((*threadResponse->records)[0]);
    }
    else {
        return nullptr;
    }
}

std::unique_ptr<ThreadRecord> DBServiceAPI::ThreadGetByContact(sys::Service *serv, uint32_t contact)
{
    std::shared_ptr<DBThreadMessageGet> msg =
        std::make_shared<DBThreadMessageGet>(MessageType::DBThreadGetForContact, contact);
    auto ret                                = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBThreadResponseMessage *threadResponse = reinterpret_cast<DBThreadResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)) {
        return std::make_unique<ThreadRecord>((*threadResponse->records)[0]);
    }
    else {
        return nullptr;
    }
}

bool DBServiceAPI::ThreadRemove(sys::Service *serv, uint32_t id)
{
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadRemove);
    msg->id                              = id;

    auto ret                                = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBThreadResponseMessage *threadResponse = reinterpret_cast<DBThreadResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::ThreadGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit)
{
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadGetLimitOffset);
    msg->offset                          = offset;
    msg->limit                           = limit;

    sys::Bus::SendUnicast(msg, service::name::db, serv);

    return true;
}

uint32_t DBServiceAPI::ThreadGetCount(sys::Service *serv)
{
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadGetCount);

    auto ret                                = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBThreadResponseMessage *threadResponse = reinterpret_cast<DBThreadResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)) {
        return threadResponse->count;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::SMSTemplateAdd(sys::Service *serv, const SMSTemplateRecord &rec)
{
    auto msg = std::make_shared<DBSMSTemplateMessage>(MessageType::DBSMSTemplateAdd, rec);

    auto ret  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    auto resp = dynamic_cast<DBSMSTemplateResponseMessage *>(ret.second.get());
    if (ret.first == sys::ReturnCodes::Success && resp != nullptr) {
        return resp->retCode;
    }

    return false;
}

bool DBServiceAPI::SMSTemplateRemove(sys::Service *serv, uint32_t id)
{
    auto msg = std::make_shared<DBSMSTemplateMessage>(MessageType::DBSMSTemplateRemove);
    msg->id  = id;

    auto ret  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    auto resp = dynamic_cast<DBSMSTemplateResponseMessage *>(ret.second.get());
    if (ret.first == sys::ReturnCodes::Success && resp != nullptr) {
        return resp->retCode;
    }

    return false;
}

bool DBServiceAPI::SMSTemplateUpdate(sys::Service *serv, const SMSTemplateRecord &rec)
{
    auto msg = std::make_shared<DBSMSTemplateMessage>(MessageType::DBSMSTemplateUpdate, rec);

    auto ret  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    auto resp = dynamic_cast<DBSMSTemplateResponseMessage *>(ret.second.get());
    if (ret.first == sys::ReturnCodes::Success && resp != nullptr) {
        return resp->retCode;
    }

    return false;
}

uint32_t DBServiceAPI::SMSTemplateGetCount(sys::Service *serv)
{
    auto msg = std::make_shared<DBSMSTemplateGetCount>();

    auto ret  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    auto resp = dynamic_cast<DBSMSTemplateResponseMessage *>(ret.second.get());
    if (ret.first == sys::ReturnCodes::Success && resp != nullptr) {
        return resp->count;
    }

    return 0;
}

bool DBServiceAPI::SMSTemplateGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit)
{
    auto msg    = std::make_shared<DBSMSTemplateMessage>(MessageType::DBSMSTemplateGetLimitOffset);
    msg->offset = offset;
    msg->limit  = limit;

    return sys::Bus::SendUnicast(msg, service::name::db, serv);
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetByName(sys::Service *serv,
                                                                           UTF8 primaryName,
                                                                           UTF8 alternativeName)
{

    ContactRecord rec;
    rec.primaryName     = primaryName;
    rec.alternativeName = alternativeName;

    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByName, rec);

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return std::move(contactResponse->records);
    }
    else {
        return std::make_unique<std::vector<ContactRecord>>();
    }
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetByID(sys::Service *serv, uint32_t contactID)
{
    ContactRecord rec;
    rec.ID = contactID;

    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByID, rec);

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return std::move(contactResponse->records);
    }
    else {
        return std::make_unique<std::vector<ContactRecord>>();
    }
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetBySpeeddial(sys::Service *serv, UTF8 speeddial)
{
    if (speeddial.length() == 0)
        return std::make_unique<std::vector<ContactRecord>>();

    ContactRecord rec;
    rec.speeddial = speeddial;

    std::shared_ptr<DBContactMessage> msg =
        std::make_shared<DBContactMessage>(MessageType::DBContactGetBySpeedDial, rec);

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return std::move(contactResponse->records);
    }
    else {
        return std::make_unique<std::vector<ContactRecord>>();
    }
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetByPhoneNumber(sys::Service *serv, UTF8 phoneNumber)
{
    ContactRecord rec;
    rec.numbers.push_back(ContactRecord::Number(phoneNumber, phoneNumber));

    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByNumber, rec);

    auto ret              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    auto *contactResponse = dynamic_cast<DBContactResponseMessage *>(ret.second.get());
    assert(contactResponse);
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return std::move(contactResponse->records);
    }
    else {
        return std::make_unique<std::vector<ContactRecord>>();
    }
}

DBServiceAPI::ContactVerificationError DBServiceAPI::verifyContact(sys::Service *serv,
                                                                   const ContactRecord &rec,
                                                                   ContactRecord &errNumPrim,
                                                                   ContactRecord &errNumAlt,
                                                                   ContactRecord &errSpeedDial)
{
    auto retSpeedDial = ContactGetBySpeeddial(serv, rec.speeddial);
    if (!retSpeedDial->empty()) {
        errSpeedDial = retSpeedDial->operator[](0);
        return (speedDialError);
    }

    if (rec.numbers.size() > 0 && rec.numbers[0].numberE164.length() > 0) {
        auto retPhone1 = ContactGetByPhoneNumber(serv, rec.numbers[0].numberE164);
        if (!retPhone1->empty()) {
            errNumPrim = retPhone1->operator[](0);
            return (primaryNumberError);
        }
    }

    if (rec.numbers.size() > 1 && rec.numbers[1].numberE164.length() > 0) {
        auto retPhone2 = ContactGetByPhoneNumber(serv, rec.numbers[1].numberE164);
        if (!retPhone2->empty()) {
            errNumAlt = retPhone2->operator[](0);
            return (secondaryNumberError);
        }
    }

    return (noError);
}

std::string DBServiceAPI::getVerificationErrorString(const ContactVerificationError err)
{
    switch (err) {
    case noError:
        return ("No error occured");
    case speedDialError:
        return ("Invalid or duplicate speed dial number");
    case primaryNumberError:
        return ("Invalid or duplicate primary number");
    case secondaryNumberError:
        return ("Invalid or duplicate secondary number");
    default:
        return "(Unknonw error)";
    }
}

bool DBServiceAPI::ContactAdd(sys::Service *serv, const ContactRecord &rec)
{
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactAdd, rec);
    msg->record.contactType               = ContactType::USER;

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::ContactRemove(sys::Service *serv, uint32_t id)
{
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactRemove);
    msg->id                               = id;

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::ContactUpdate(sys::Service *serv, const ContactRecord &rec)
{
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactUpdate, rec);

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::ContactBlock(sys::Service *serv, uint32_t id, const bool shouldBeBlocked)
{
    std::shared_ptr<DBContactBlock> msg =
        std::make_shared<DBContactBlock>(MessageType::DBContactBlock, id, shouldBeBlocked);

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

uint32_t DBServiceAPI::ContactGetCount(sys::Service *serv, bool favourites)
{
    std::shared_ptr<DBContactMessage> msg =
        std::make_shared<DBContactMessage>(MessageType::DBContactGetCount, ContactRecord{}, favourites);

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return contactResponse->count;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::ContactGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit)
{
    std::shared_ptr<DBContactMessage> msg =
        std::make_shared<DBContactMessage>(MessageType::DBContactGetLimitOffset, ContactRecord{});
    msg->offset = offset;
    msg->limit  = limit;

    sys::Bus::SendUnicast(msg, ServiceDB::serviceName, serv);
    return true;
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactSearch(sys::Service *serv,
                                                                        UTF8 primaryName,
                                                                        UTF8 alternativeName,
                                                                        UTF8 number)
{
    std::shared_ptr<DBContactSearchMessage> msg =
        std::make_shared<DBContactSearchMessage>(MessageType::DBContactSearch,
                                                 (primaryName.length() > 0) ? primaryName.c_str() : "",
                                                 (alternativeName.length() > 0) ? alternativeName.c_str() : "",
                                                 (number.length() > 0) ? number.c_str() : "");

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return std::move(contactResponse->records);
    }
    else {
        return std::make_unique<std::vector<ContactRecord>>();
    }
}

bool DBServiceAPI::AlarmAdd(sys::Service *serv, const AlarmsRecord &rec)
{
    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmAdd, rec);

    auto ret                              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBAlarmResponseMessage *alarmResponse = reinterpret_cast<DBAlarmResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::AlarmRemove(sys::Service *serv, uint32_t id)
{
    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmRemove);
    msg->id                             = id;

    auto ret                              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBAlarmResponseMessage *alarmResponse = reinterpret_cast<DBAlarmResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::AlarmUpdate(sys::Service *serv, const AlarmsRecord &rec)
{
    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmUpdate, rec);

    auto ret                              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBAlarmResponseMessage *alarmResponse = reinterpret_cast<DBAlarmResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

uint32_t DBServiceAPI::AlarmGetCount(sys::Service *serv)
{
    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmGetCount);

    auto ret                              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBAlarmResponseMessage *alarmResponse = reinterpret_cast<DBAlarmResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)) {
        return alarmResponse->count;
    }
    else {
        return false;
    }
}

std::unique_ptr<std::vector<AlarmsRecord>> DBServiceAPI::AlarmGetLimitOffset(sys::Service *serv,
                                                                             uint32_t offset,
                                                                             uint32_t limit)
{
    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmGetLimitOffset);
    msg->offset                         = offset;
    msg->limit                          = limit;

    auto ret                              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBAlarmResponseMessage *alarmResponse = reinterpret_cast<DBAlarmResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)) {
        return std::move(alarmResponse->records);
    }
    else {
        return std::make_unique<std::vector<AlarmsRecord>>();
    }
}

AlarmsRecord DBServiceAPI::AlarmGetNext(sys::Service *serv, time_t time)
{

    std::shared_ptr<DBAlarmMessage> msg   = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmGetNext);
    msg->time                             = time;
    auto ret                              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBAlarmResponseMessage *alarmResponse = reinterpret_cast<DBAlarmResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)) {
        return std::move((*alarmResponse->records)[0]);
    }
    else {
        return AlarmsRecord{};
    }
}

bool DBServiceAPI::NotesAdd(sys::Service *serv, const NotesRecord &rec)
{
    std::shared_ptr<DBNotesMessage> msg = std::make_shared<DBNotesMessage>(MessageType::DBNotesAdd, rec);

    auto ret                              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBNotesResponseMessage *notesResponse = reinterpret_cast<DBNotesResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (notesResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::NotesRemove(sys::Service *serv, uint32_t id)
{
    std::shared_ptr<DBNotesMessage> msg = std::make_shared<DBNotesMessage>(MessageType::DBNotesRemove);
    msg->id                             = id;

    auto ret                              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBNotesResponseMessage *notesResponse = reinterpret_cast<DBNotesResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (notesResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::NotesUpdate(sys::Service *serv, const NotesRecord &rec)
{
    std::shared_ptr<DBNotesMessage> msg = std::make_shared<DBNotesMessage>(MessageType::DBNotesUpdate, rec);

    auto ret                              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBNotesResponseMessage *notesResponse = reinterpret_cast<DBNotesResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (notesResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

uint32_t DBServiceAPI::NotesGetCount(sys::Service *serv)
{
    std::shared_ptr<DBNotesMessage> msg = std::make_shared<DBNotesMessage>(MessageType::DBNotesGetCount);

    auto ret                              = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBNotesResponseMessage *notesResponse = reinterpret_cast<DBNotesResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (notesResponse->retCode == true)) {
        return notesResponse->count;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::NotesGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit)
{
    std::shared_ptr<DBNotesMessage> msg = std::make_shared<DBNotesMessage>(MessageType::DBNotesGetLimitOffset);
    msg->offset                         = offset;
    msg->limit                          = limit;

    sys::Bus::SendUnicast(msg, service::name::db, serv);
    return true;
}

CalllogRecord DBServiceAPI::CalllogAdd(sys::Service *serv, const CalllogRecord &rec)
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogAdd, rec);

    LOG_DEBUG("CalllogAdd %s", utils::to_string(rec).c_str());

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBCalllogResponseMessage *calllogResponse = reinterpret_cast<DBCalllogResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (calllogResponse->retCode == true)) {
        auto records = *calllogResponse->records;
        if (!records.empty()) {
            return records[0];
        }
    }

    return CalllogRecord();
}

bool DBServiceAPI::CalllogRemove(sys::Service *serv, uint32_t id)
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogRemove);
    msg->id                               = id;

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBCalllogResponseMessage *calllogResponse = reinterpret_cast<DBCalllogResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (calllogResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

bool DBServiceAPI::CalllogUpdate(sys::Service *serv, const CalllogRecord &rec)
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogUpdate, rec);

    LOG_DEBUG("CalllogUpdate %s", utils::to_string(rec).c_str());

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBCalllogResponseMessage *calllogResponse = reinterpret_cast<DBCalllogResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (calllogResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
}

uint32_t DBServiceAPI::CalllogGetCount(sys::Service *serv, EntryState state)
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogGetCount>(state);

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBCalllogResponseMessage *calllogResponse = reinterpret_cast<DBCalllogResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (calllogResponse->retCode == true)) {
        return calllogResponse->count;
    }
    else {
        return 0;
    }
}

bool DBServiceAPI::CalllogGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit)
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogGetLimitOffset);
    msg->offset                           = offset;
    msg->limit                            = limit;

    sys::Bus::SendUnicast(msg, service::name::db, serv);
    return true;
}

uint32_t DBServiceAPI::GetCountryCodeByMCC(sys::Service *serv, uint32_t mcc)
{
    std::shared_ptr<DBCountryCodeMessage> msg =
        std::make_shared<DBCountryCodeMessage>(MessageType::DBCountryCode, mcc, 0);

    auto ret                               = sys::Bus::SendUnicast(msg, service::name::db, serv, 5000);
    DBCountryCodeResponseMessage *response = reinterpret_cast<DBCountryCodeResponseMessage *>(ret.second.get());
    if (ret.first == sys::ReturnCodes::Success) {
        return (response->row.country_code);
    }
    else {
        return (0);
    }
}
