// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-db/DBServiceAPI.hpp"
#include "service-db/DBContactMessage.hpp"
#include "service-db/DBCalllogMessage.hpp"
#include "service-db/DBServiceMessage.hpp"
#include "service-db/QueryMessage.hpp"
#include "service-db/QuotesMessages.hpp"
#include "service-db/DBServiceName.hpp"

#include <CalllogRecord.hpp>
#include <ContactRecord.hpp>
#include <MessageType.hpp>
#include <PhoneNumber.hpp>
#include <SMSTemplateRecord.hpp>
#include <system/Common.hpp>
#include <Service/Service.hpp>
#include <Tables/CountryCodesTable.hpp>
#include <ThreadRecord.hpp>
#include <Utils.hpp>
#include <log/log.hpp>
#include <queries/messages/threads/QueryThreadGetByNumber.hpp>
#include <queries/phonebook/QueryNumberGetByID.hpp>

#include <utility>
#include <cassert>

namespace constants
{
    constexpr std::uint32_t DefaultTimeoutInMs = 10000U;
    constexpr std::uint32_t BackupTimeoutInMs  = 120000U;
} // namespace constants

struct NotesRecord;

auto DBServiceAPI::ContactGetByID(sys::Service *serv, uint32_t contactID) -> std::unique_ptr<std::vector<ContactRecord>>
{
    ContactRecord rec;
    rec.ID = contactID;

    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByID, rec);
    return ContactGetByIDCommon(serv, std::move(msg));
}

auto DBServiceAPI::ContactGetByIDCommon(sys::Service *serv, std::shared_ptr<DBContactMessage> contactMsg)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    auto ret             = serv->bus.sendUnicastSync(contactMsg, service::name::db, constants::DefaultTimeoutInMs);
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

auto DBServiceAPI::MatchContactByPhoneNumber(sys::Service *serv,
                                             const utils::PhoneNumber::View &numberView,
                                             const std::uint32_t contactIDToOmit) -> std::unique_ptr<ContactRecord>
{
    auto msg = std::make_shared<DBMatchContactNumberBesidesOfContactIDMessage>(numberView, contactIDToOmit);

    auto ret             = serv->bus.sendUnicastSync(std::move(msg), service::name::db, constants::DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactNumberResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr || contactResponse->retCode != sys::ReturnCodes::Success) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return nullptr;
    }
    return std::move(contactResponse->contact);
}

auto DBServiceAPI::MatchContactByNumberID(sys::Service *serv, std::uint32_t numberID) -> std::unique_ptr<ContactRecord>
{
    auto msg             = std::make_shared<DBMatchContactByNumberIDMessage>(numberID);
    auto ret             = serv->bus.sendUnicastSync(std::move(msg), service::name::db, constants::DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactNumberResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr || contactResponse->retCode != sys::ReturnCodes::Success) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return nullptr;
    }
    return std::move(contactResponse->contact);
}

auto DBServiceAPI::NumberByID(sys::Service *serv, std::uint32_t numberID) -> utils::PhoneNumber::View
{
    auto msg = std::make_unique<db::query::NumberGetByID>(numberID);
    const auto [status, response] = DBServiceAPI::GetQueryWithReply(
        serv, db::Interface::Name::Contact, std::move(msg), constants::DefaultTimeoutInMs);
    if (status != sys::ReturnCodes::Success || !response) {
        return utils::PhoneNumber::View{};
    }
    auto queryResponse = dynamic_cast<db::QueryResponse *>(response.get());
    if (queryResponse == nullptr) {
        return utils::PhoneNumber::View{};
    }
    auto result           = queryResponse->getResult();
    auto numberIdResponse = dynamic_cast<db::query::NumberGetByIDResult *>(result.get());
    return numberIdResponse->getNumber();
}

auto DBServiceAPI::IsContactInFavourites(sys::Service *serv, const utils::PhoneNumber::View &numberView) -> bool
{
    auto msg = std::make_shared<DBContactNumberMessage>(numberView);

    auto ret             = serv->bus.sendUnicastSync(std::move(msg), service::name::db, constants::DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactNumberResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr || contactResponse->retCode != sys::ReturnCodes::Success) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return false;
    }
    return contactResponse->contact != nullptr && contactResponse->contact->isOnFavourites();
}

auto DBServiceAPI::IsContactInEmergency(sys::Service *serv, const utils::PhoneNumber::View &numberView) -> bool
{
    auto msg = std::make_shared<DBContactNumberMessage>(numberView);

    auto ret             = serv->bus.sendUnicastSync(std::move(msg), service::name::db, constants::DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactNumberResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr || contactResponse->retCode != sys::ReturnCodes::Success) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return false;
    }
    return contactResponse->contact != nullptr && contactResponse->contact->isOnIce();
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
        auto retPhone1 = MatchContactByPhoneNumber(serv, rec.numbers[0].number, rec.ID);
        if (retPhone1) {
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

auto DBServiceAPI::ContactAdd(sys::Service *serv, const ContactRecord &rec) -> std::optional<ContactRecord>
{
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactAdd, rec);

    auto ret             = serv->bus.sendUnicastSync(msg, service::name::db, constants::DefaultTimeoutInMs);
    auto contactResponse = dynamic_cast<DBContactResponseMessage *>(ret.second.get());
    if (contactResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return std::nullopt;
    }
    if ((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode != 0)) {
        auto records = *contactResponse->records;
        if (!records.empty()) {
            return records[0];
        }
    }
    return std::nullopt;
}

auto DBServiceAPI::ContactRemove(sys::Service *serv, uint32_t id) -> bool
{
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactRemove);
    msg->id                               = id;

    auto ret             = serv->bus.sendUnicastSync(msg, service::name::db, constants::DefaultTimeoutInMs);
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

    auto ret             = serv->bus.sendUnicastSync(msg, service::name::db, constants::DefaultTimeoutInMs);
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

auto DBServiceAPI::CalllogAdd(sys::Service *serv, const CalllogRecord &rec) -> CalllogRecord
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogAdd, rec);

    auto ret             = serv->bus.sendUnicastSync(msg, service::name::db, constants::DefaultTimeoutInMs);
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

    auto ret             = serv->bus.sendUnicastSync(msg, service::name::db, constants::DefaultTimeoutInMs);
    auto calllogResponse = dynamic_cast<DBCalllogResponseMessage *>(ret.second.get());
    if (calllogResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return false;
    }
    return ((ret.first == sys::ReturnCodes::Success) && (calllogResponse->retCode != 0));
}

auto DBServiceAPI::CalllogUpdate(sys::Service *serv, const CalllogRecord &rec) -> bool
{
    std::shared_ptr<DBCalllogMessage> msg = std::make_shared<DBCalllogMessage>(MessageType::DBCalllogUpdate, rec);

    auto ret             = serv->bus.sendUnicastSync(msg, service::name::db, constants::DefaultTimeoutInMs);
    auto calllogResponse = dynamic_cast<DBCalllogResponseMessage *>(ret.second.get());
    if (calllogResponse == nullptr) {
        LOG_ERROR("DB response error, return code: %s", c_str(ret.first));
        return false;
    }
    return ((ret.first == sys::ReturnCodes::Success) && (calllogResponse->retCode != 0));
}

auto DBServiceAPI::DBBackup(sys::Service *serv, std::string backupPath) -> bool
{
    LOG_INFO("DBBackup %s", backupPath.c_str());

    std::shared_ptr<DBServiceMessageBackup> msg =
        std::make_shared<DBServiceMessageBackup>(MessageType::DBServiceBackup, backupPath);

    auto ret = serv->bus.sendUnicastSync(msg, service::name::db, constants::BackupTimeoutInMs);
    if (auto retMsg = dynamic_cast<DBServiceResponseMessage *>(ret.second.get()); retMsg) {
        return retMsg->retCode;
    }
    LOG_ERROR("DBBackup error, return code: %s", c_str(ret.first));
    return false;
}

bool DBServiceAPI::AddSMS(sys::Service *serv, const SMSRecord &record, std::unique_ptr<db::QueryListener> &&listener)
{
    auto query = std::make_unique<db::query::SMSAdd>(record);
    query->setQueryListener(std::move(listener));
    const auto [succeed, _] = DBServiceAPI::GetQuery(serv, db::Interface::Name::SMS, std::move(query));
    return succeed;
}

void DBServiceAPI::InformLanguageChanged(sys::Service *serv)
{
    auto query = std::make_unique<Quotes::Messages::InformLanguageChangeRequest>();
    DBServiceAPI::GetQuery(serv, db::Interface::Name::Quotes, std::move(query));
}
