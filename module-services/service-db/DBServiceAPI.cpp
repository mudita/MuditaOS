// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-db/DBServiceAPI.hpp"
#include "service-db/DBThreadMessage.hpp"
#include "service-db/DBContactMessage.hpp"
#include "service-db/DBNotesMessage.hpp"
#include "service-db/DBCalllogMessage.hpp"
#include "service-db/DBCountryCodeMessage.hpp"
#include "service-db/DBServiceMessage.hpp"
#include "service-db/QueryMessage.hpp"
#include "service-db/DBServiceName.hpp"

#include <CalllogRecord.hpp>
#include <ContactRecord.hpp>
#include <MessageType.hpp>
#include <PhoneNumber.hpp>
#include <SMSTemplateRecord.hpp>
#include <Service/Bus.hpp>
#include <Service/Common.hpp>
#include <Tables/CountryCodesTable.hpp>
#include <ThreadRecord.hpp>
#include <Utils.hpp>
#include <log/log.hpp>
#include <queries/messages/threads/QueryThreadGetByNumber.hpp>

#include <utility>
#include <cassert>

namespace sys
{
    class Service;
} // namespace sys
struct NotesRecord;

auto DBServiceAPI::ThreadGetByNumber(sys::Service *serv,
                                     const utils::PhoneNumber::View &phoneNumber,
                                     std::uint32_t timeout) -> std::unique_ptr<ThreadRecord>
{
    auto [code, msg] = DBServiceAPI::GetQueryWithReply(
        serv, db::Interface::Name::SMSThread, std::make_unique<db::query::ThreadGetByNumber>(phoneNumber), timeout);

    if (code == sys::ReturnCodes::Success && msg != nullptr) {
        auto queryResponse = dynamic_cast<db::QueryResponse *>(msg.get());
        assert(queryResponse != nullptr);

        auto threadResponse = queryResponse->getResult();
        auto threadResult   = dynamic_cast<db::query::ThreadGetByNumberResult *>(threadResponse.get());
        assert(threadResult != nullptr);

        return std::make_unique<ThreadRecord>(threadResult->getThread());
    }

    return nullptr;
}

auto DBServiceAPI::ThreadGetCount(sys::Service *serv, EntryState state) -> uint32_t
{
    auto msg = std::make_shared<DBThreadGetCountMessage>(state);

    auto ret            = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto threadResponse = dynamic_cast<DBThreadResponseMessage *>(ret.second.get());
    if (threadResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return 0;
    }
    if ((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == 1)) {
        return threadResponse->count;
    }
    return 0;
}

auto DBServiceAPI::ContactGetByID(sys::Service *serv, uint32_t contactID) -> std::unique_ptr<std::vector<ContactRecord>>
{
    ContactRecord rec;
    rec.ID = contactID;

    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByID, rec);
    return ContactGetByIDCommon(serv, std::move(msg));
}

auto DBServiceAPI::ContactGetByIDWithTemporary(sys::Service *serv, uint32_t contactID)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    ContactRecord rec;
    rec.ID                                = contactID;
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByID, rec);
    msg->withTemporary                    = true;
    return ContactGetByIDCommon(serv, std::move(msg));
}

auto DBServiceAPI::ContactGetByIDCommon(sys::Service *serv, std::shared_ptr<DBContactMessage> contactMsg)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    auto ret             = sys::Bus::SendUnicast(contactMsg, service::name::db, serv, DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return nullptr;
    }
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode != 0)) {
        return std::move(contactResponse->records);
    }
    return std::make_unique<std::vector<ContactRecord>>();
}

auto DBServiceAPI::ContactGetBySpeeddial(sys::Service *serv, UTF8 speeddial)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    if (speeddial.length() == 0) {
        return std::make_unique<std::vector<ContactRecord>>();
    }

    ContactRecord rec;
    rec.speeddial = speeddial;

    std::shared_ptr<DBContactMessage> msg =
        std::make_shared<DBContactMessage>(MessageType::DBContactGetBySpeedDial, rec);

    return ContactGetByIDCommon(serv, msg);
}

auto DBServiceAPI::ContactGetByPhoneNumber(sys::Service *serv, UTF8 phoneNumber)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    ContactRecord rec;
    auto number = ContactRecord::Number(utils::PhoneNumber(phoneNumber).getView(), ContactNumberType ::PAGER);
    rec.numbers.push_back(number);

    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByNumber, rec);

    return ContactGetByIDCommon(serv, msg);
}

auto DBServiceAPI::MatchContactByPhoneNumber(sys::Service *serv, const utils::PhoneNumber::View &numberView)
    -> std::unique_ptr<ContactRecord>
{
    auto msg = std::make_shared<DBContactNumberMessage>(numberView);

    auto ret             = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactNumberResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return nullptr;
    }
    return std::move(contactResponse->contact);
}

auto DBServiceAPI::verifyContact(sys::Service *serv, const ContactRecord &rec)
    -> DBServiceAPI::ContactVerificationResult
{

    if (rec.primaryName.length() == 0 && rec.alternativeName.length() == 0 && rec.numbers.empty() &&
        rec.mail.length() == 0 && rec.address.length() == 0 && rec.note.length() == 0) {
        return ContactVerificationResult::emptyContact;
    }

    auto retSpeedDial = ContactGetBySpeeddial(serv, rec.speeddial);
    if (!retSpeedDial->empty() && (*retSpeedDial)[0].ID != rec.ID) {
        return ContactVerificationResult::speedDialDuplicate;
    }

    if (rec.numbers.size() > 0 && rec.numbers[0].number.getEntered().size() > 0) {
        auto retPhone1 = MatchContactByPhoneNumber(serv, rec.numbers[0].number);
        if (retPhone1 && retPhone1->ID != rec.ID) {
            if (retPhone1->isTemporary()) {
                return ContactVerificationResult::temporaryContactExists;
            }
            return ContactVerificationResult::primaryNumberDuplicate;
        }
    }

    if (rec.numbers.size() > 1 && rec.numbers[1].number.getEntered().size() > 0) {
        auto retPhone2 = MatchContactByPhoneNumber(serv, rec.numbers[1].number);
        if (retPhone2 && retPhone2->ID != rec.ID) {
            if (retPhone2->isTemporary()) {
                return ContactVerificationResult::temporaryContactExists;
            }
            return ContactVerificationResult::secondaryNumberDuplicate;
        }
    }

    return ContactVerificationResult::success;
}

auto DBServiceAPI::ContactAdd(sys::Service *serv, const ContactRecord &rec) -> bool
{
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactAdd, rec);

    auto ret             = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return false;
    }
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode != 0)) {
        return true;
    }
    return false;
}

auto DBServiceAPI::ContactRemove(sys::Service *serv, uint32_t id) -> bool
{
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactRemove);
    msg->id                               = id;

    auto ret             = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return false;
    }
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode != 0)) {
        return true;
    }
    return false;
}

auto DBServiceAPI::ContactUpdate(sys::Service *serv, const ContactRecord &rec) -> bool
{
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactUpdate, rec);

    auto ret             = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return false;
    }
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode != 0)) {
        return true;
    }
    return false;
}

auto DBServiceAPI::ContactSearch(sys::Service *serv, UTF8 primaryName, UTF8 alternativeName, UTF8 number)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    std::shared_ptr<DBContactSearchMessage> msg =
        std::make_shared<DBContactSearchMessage>(MessageType::DBContactSearch,
                                                 (primaryName.length() > 0) ? primaryName.c_str() : "",
                                                 (alternativeName.length() > 0) ? alternativeName.c_str() : "",
                                                 (number.length() > 0) ? number.c_str() : "");

    auto ret             = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return nullptr;
    }
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode != 0)) {
        return std::move(contactResponse->records);
    }
    return std::make_unique<std::vector<ContactRecord>>();
}

auto DBServiceAPI::CalllogAdd(sys::Service *serv, const CalllogRecord &rec) -> CalllogRecord
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogAdd, rec);

    LOG_DEBUG("CalllogAdd %s", utils::to_string(rec).c_str());

    auto ret             = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto calllogResponse = dynamic_cast<DBCalllogResponseMessage *>(ret.second.get());
    if (calllogResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return CalllogRecord();
    }
    if ((ret.first == sys::ReturnCodes::Success) && (calllogResponse->retCode != 0)) {
        auto records = *calllogResponse->records;
        if (!records.empty()) {
            return records[0];
        }
    }

    return CalllogRecord();
}

auto DBServiceAPI::CalllogRemove(sys::Service *serv, uint32_t id) -> bool
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogRemove);
    msg->id                               = id;

    auto ret             = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto calllogResponse = dynamic_cast<DBCalllogResponseMessage *>(ret.second.get());
    if (calllogResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return false;
    }
    if ((ret.first == sys::ReturnCodes::Success) && (calllogResponse->retCode != 0)) {
        return true;
    }
    return false;
}

auto DBServiceAPI::CalllogUpdate(sys::Service *serv, const CalllogRecord &rec) -> bool
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogUpdate, rec);

    LOG_DEBUG("CalllogUpdate %s", utils::to_string(rec).c_str());

    auto ret             = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto calllogResponse = dynamic_cast<DBCalllogResponseMessage *>(ret.second.get());
    if (calllogResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return false;
    }
    if ((ret.first == sys::ReturnCodes::Success) && (calllogResponse->retCode != 0)) {
        return true;
    }
    return false;
}

auto DBServiceAPI::CalllogGetCount(sys::Service *serv, EntryState state) -> uint32_t
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogGetCount>(state);

    auto ret             = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto calllogResponse = dynamic_cast<DBCalllogResponseMessage *>(ret.second.get());
    if (calllogResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return 0;
    }
    if ((ret.first == sys::ReturnCodes::Success) && (calllogResponse->retCode != 0)) {
        return calllogResponse->count;
    }
    return 0;
}

auto DBServiceAPI::CalllogGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit) -> bool
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogGetLimitOffset);
    msg->offset                           = offset;
    msg->limit                            = limit;

    sys::Bus::SendUnicast(msg, service::name::db, serv);
    return true;
}

auto DBServiceAPI::GetCountryCodeByMCC(sys::Service *serv, uint32_t mcc) -> uint32_t
{
    std::shared_ptr<DBCountryCodeMessage> msg =
        std::make_shared<DBCountryCodeMessage>(MessageType::DBCountryCode, mcc, 0);

    auto ret      = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    auto response = dynamic_cast<DBCountryCodeResponseMessage *>(ret.second.get());
    if (response == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return 0;
    }
    if (ret.first == sys::ReturnCodes::Success) {
        return (response->row.country_code);
    }
    return 0;
}

auto DBServiceAPI::DBBackup(sys::Service *serv, std::string backupPath) -> bool
{
    LOG_INFO("DBBackup %s", backupPath.c_str());

    std::shared_ptr<DBServiceMessageBackup> msg =
        std::make_shared<DBServiceMessageBackup>(MessageType::DBServiceBackup, backupPath);

    auto ret = sys::Bus::SendUnicast(msg, service::name::db, serv, DefaultTimeoutInMs);
    if (ret.first == sys::ReturnCodes::Success) {
        return true;
    }
    LOG_ERROR("DBBackup error, return code: %s", c_str(ret.first));
    return false;
}
