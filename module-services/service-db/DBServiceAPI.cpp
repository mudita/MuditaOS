// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-db/DBServiceAPI.hpp"
#include "service-db/DBThreadMessage.hpp"
#include "service-db/DBSMSMessage.hpp"
#include "service-db/DBSMSTemplateMessage.hpp"
#include "service-db/DBContactMessage.hpp"
#include "service-db/DBNotesMessage.hpp"
#include "service-db/DBCalllogMessage.hpp"
#include "service-db/DBCountryCodeMessage.hpp"
#include "service-db/DBServiceMessage.hpp"
#include "service-db/QueryMessage.hpp"
#include "service-db/DBServiceName.hpp"

#include <AlarmsRecord.hpp>
#include <CalllogRecord.hpp>
#include <Common/Query.hpp>
#include <ContactRecord.hpp>
#include <MessageType.hpp>
#include <PhoneNumber.hpp>
#include <SMSRecord.hpp>
#include <SMSTemplateRecord.hpp>
#include <Service/Bus.hpp>
#include <Service/Common.hpp>
#include <Tables/CountryCodesTable.hpp>
#include <Tables/Record.hpp>
#include <ThreadRecord.hpp>
#include <Utils.hpp>
#include <ext/alloc_traits.h>
#include <log/log.hpp>
#include <queries/messages/threads/QueryThreadGetByNumber.hpp>
#include <queries/phonebook/QueryNumberGetByID.hpp>

#include <utility>
#include <cassert>

namespace sys
{
    class Service;
} // namespace sys
struct NotesRecord;

std::unique_ptr<ThreadRecord> DBServiceAPI::ThreadGetByNumber(sys::Service *serv,
                                                              const utils::PhoneNumber::View &phoneNumber,
                                                              std::uint32_t timeout)
{
    auto [code, msg] = DBServiceAPI::GetQueryWithReply(
        serv, db::Interface::Name::SMSThread, std::make_unique<db::query::ThreadGetByNumber>(phoneNumber), timeout);

    if (code == sys::ReturnCodes::Success && msg != nullptr) {
        auto queryResponse = dynamic_cast<db::QueryResponse *>(msg.get());
        assert(queryResponse != nullptr);

        auto threadResponse = queryResponse->getResult();
        auto threadResult   = dynamic_cast<db::query::ThreadGetByNumberResult *>(threadResponse.get());
        assert(threadResult != nullptr);

        return std::make_unique<ThreadRecord>(std::move(threadResult->getThread()));
    }

    return nullptr;
}

bool DBServiceAPI::ThreadGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit)
{
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadGetLimitOffset);
    msg->offset                          = offset;
    msg->limit                           = limit;

    sys::Bus::SendUnicast(msg, service::name::db, serv);

    return true;
}

uint32_t DBServiceAPI::ThreadGetCount(sys::Service *serv, EntryState state)
{
    auto msg = std::make_shared<DBThreadGetCountMessage>(state);

    auto ret                                = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    DBThreadResponseMessage *threadResponse = reinterpret_cast<DBThreadResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)) {
        return threadResponse->count;
    }
    else {
        return false;
    }
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetByID(sys::Service *serv, uint32_t contactID)
{
    ContactRecord rec;
    rec.ID = contactID;

    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByID, rec);
    return std::move(ContactGetByIDCommon(serv, std::move(msg)));
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetByIDWithTemporary(sys::Service *serv,
                                                                                      uint32_t contactID)
{
    ContactRecord rec;
    rec.ID                                = contactID;
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByID, rec);
    msg->withTemporary                    = true;
    return std::move(ContactGetByIDCommon(serv, std::move(msg)));
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetByIDCommon(
    sys::Service *serv, std::shared_ptr<DBContactMessage> contactMsg)
{
    auto ret = sys::Bus::SendUnicast(contactMsg, service::name::db, serv, DefaultTimeoutInMs);
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

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
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
    auto number = ContactRecord::Number(utils::PhoneNumber(phoneNumber).getView(), ContactNumberType ::PAGER);
    rec.numbers.push_back(number);

    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByNumber, rec);

    auto ret              = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto *contactResponse = dynamic_cast<DBContactResponseMessage *>(ret.second.get());

    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return std::move(contactResponse->records);
    }
    else {
        return std::make_unique<std::vector<ContactRecord>>();
    }
}

std::unique_ptr<ContactRecord> DBServiceAPI::MatchContactByPhoneNumber(sys::Service *serv,
                                                                       const utils::PhoneNumber::View &numberView)
{
    auto msg = std::make_shared<DBContactNumberMessage>(numberView);

    auto ret              = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto *contactResponse = dynamic_cast<DBContactNumberResponseMessage *>(ret.second.get());
    assert(contactResponse);
    return std::move(contactResponse->contact);
}

DBServiceAPI::ContactVerificationError DBServiceAPI::verifyContact(sys::Service *serv, const ContactRecord &rec)
{

    if (rec.primaryName.length() == 0 && rec.alternativeName.length() == 0 && rec.numbers.empty() &&
        rec.mail.length() == 0 && rec.address.length() == 0 && rec.note.length() == 0) {
        return emptyContactError;
    }

    auto retSpeedDial = ContactGetBySpeeddial(serv, rec.speeddial);
    if (!retSpeedDial->empty() && (*retSpeedDial)[0].ID != rec.ID) {
        return speedDialError;
    }

    if (rec.numbers.size() > 0 && rec.numbers[0].number.getEntered().size() > 0) {
        auto retPhone1 = MatchContactByPhoneNumber(serv, rec.numbers[0].number);
        if (retPhone1 && retPhone1->ID != rec.ID) {
            return primaryNumberError;
        }
    }

    if (rec.numbers.size() > 1 && rec.numbers[1].number.getEntered().size() > 0) {
        auto retPhone2 = MatchContactByPhoneNumber(serv, rec.numbers[1].number);
        if (retPhone2 && retPhone2->ID != rec.ID) {
            return secondaryNumberError;
        }
    }

    return noError;
}

std::string DBServiceAPI::getVerificationErrorString(const ContactVerificationError err)
{
    switch (err) {
    case noError:
        return "No error occurred";
    case emptyContactError:
        return "Contact record is empty";
    case speedDialError:
        return "Invalid or duplicate speed dial number";
    case primaryNumberError:
        return "Invalid or duplicate primary number";
    case secondaryNumberError:
        return "Invalid or duplicate secondary number";
    default:
        return "Unknown error";
    }
}

bool DBServiceAPI::ContactAdd(sys::Service *serv, const ContactRecord &rec)
{
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactAdd, rec);

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
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

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
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

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return true;
    }
    else {
        return false;
    }
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

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    DBContactResponseMessage *contactResponse = reinterpret_cast<DBContactResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)) {
        return std::move(contactResponse->records);
    }
    else {
        return std::make_unique<std::vector<ContactRecord>>();
    }
}

CalllogRecord DBServiceAPI::CalllogAdd(sys::Service *serv, const CalllogRecord &rec)
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogAdd, rec);

    LOG_DEBUG("CalllogAdd %s", utils::to_string(rec).c_str());

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
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

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
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

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
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

    auto ret                                  = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
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

    auto ret                               = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    DBCountryCodeResponseMessage *response = reinterpret_cast<DBCountryCodeResponseMessage *>(ret.second.get());
    if (ret.first == sys::ReturnCodes::Success) {
        return (response->row.country_code);
    }
    else {
        return (0);
    }
}

bool DBServiceAPI::DBBackup(sys::Service *serv, std::string backupPath)
{
    LOG_INFO("DBBackup %s", backupPath.c_str());

    std::shared_ptr<DBServiceMessageBackup> msg =
        std::make_shared<DBServiceMessageBackup>(MessageType::DBServiceBackup, backupPath);

    auto ret = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    if (ret.first == sys::ReturnCodes::Success) {
        return true;
    }
    else {
        LOG_ERROR("DBBackup error, return code: %s", c_str(ret.first));
        return false;
    }
}
