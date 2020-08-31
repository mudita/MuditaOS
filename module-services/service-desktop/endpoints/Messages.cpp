#include "Messages.hpp"
#include "BaseInterface.hpp"
#include "Common/Query.hpp"
#include "ParserUtils.hpp"
#include "PhoneNumber.hpp"
#include "SMSRecord.hpp"
#include "SMSTemplateRecord.hpp"
#include "Service/Common.hpp"
#include "ThreadRecord.hpp"
#include "api/DBServiceAPI.hpp"
#include "log/log.hpp"
#include "phonenumbers/logger.h"
#include "queries/sms/QuerySMSGetByContactID.hpp"
#include "queries/sms/QuerySMSGetByID.hpp"
#include "queries/sms/QuerySMSGetByText.hpp"
#include "queries/sms/QuerySMSSearch.hpp"
#include "queries/sms/QuerySMSTemplateGetByID.hpp"
#include "queries/sms/QuerySmsThreadMarkAsRead.hpp"
#include "utf8/UTF8.hpp"
#include "json/json11.hpp"
#include <memory>

using namespace ParserStateMachine;

auto MessageHelper::getLastUUID() -> uint32_t
{
    auto uuid = uuidQueue.front();
    uuidQueue.pop();
    return uuid;
}

auto MessageHelper::to_json(SMSRecord record) -> json11::Json
{

    auto recordEntry = json11::Json::object{{json::messages::contactID, static_cast<int>(record.contactID)},
                                            {json::messages::date, static_cast<int>(record.date)},
                                            {json::messages::dateSent, static_cast<int>(record.dateSent)},
                                            {json::messages::id, static_cast<int>(record.ID)},
                                            {json::messages::messageBody, record.body.c_str()},
                                            {json::messages::phoneNumber, record.number.getEntered()},
                                            {json::messages::type, static_cast<int>(record.type)},
                                            {json::messages::threadID, static_cast<int>(record.threadID)}};
    return recordEntry;
}

auto MessageHelper::to_json(SMSTemplateRecord record) -> json11::Json
{

    auto recordEntry = json11::Json::object{{json::messages::id, static_cast<int>(record.ID)},
                                            {json::messages::templateText, record.text.c_str()}};
    return recordEntry;
}

auto MessageHelper::to_json(ThreadRecord record) -> json11::Json
{

    auto recordEntry =
        json11::Json::object{{json::messages::contactID, static_cast<int>(record.contactID)},
                             {json::messages::date, static_cast<int>(record.date)},
                             {json::messages::thread::msgCount, static_cast<int>(record.msgCount)},
                             {json::messages::id, static_cast<int>(record.ID)},
                             {json::messages::thread::snippet, record.snippet.c_str()},
                             {json::messages::isUnread, record.isUnread()},
                             {json::messages::type, static_cast<int>(record.type)},
                             {json::messages::thread::unreadMsgCount, static_cast<int>(record.unreadMsgCount)}};
    return recordEntry;
}

auto MessageHelper::from_json(json11::Json msgJson) -> SMSTemplateRecord
{
    SMSTemplateRecord record;

    record.text = UTF8(msgJson[json::messages::templateText].string_value());

    return record;
}

auto MessageHelper::requestDataFromDB(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes
{
    uuidQueue.push(uuid);

    if (getJSON[json::messages::msgTemplate].bool_value() == true) {
        return requestTemplate(getJSON, uuid);
    }
    else {
        return requestSMS(getJSON, uuid);
    }
}

auto MessageHelper::requestSMS(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes
{
    if (getJSON[json::messages::count].bool_value() == true) // get messages count
    {
        DBServiceAPI::SMSGetCount(ownerServicePtr);
    }
    else if (getJSON[json::messages::id].int_value() != 0) { // messages search

        auto query = std::make_unique<db::query::SMSGetByID>(getJSON[json::messages::id].int_value());
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMS, std::move(query));
    }
    else if (getJSON[json::messages::contactID].int_value() != 0) {

        auto query = std::make_unique<db::query::SMSGetByContactID>(getJSON[json::messages::contactID].int_value());
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMS, std::move(query));
    }
    else if (getJSON[json::messages::messageBody].string_value().empty() == false) {
        auto query = std::make_unique<db::query::SMSGetByText>(getJSON[json::messages::messageBody].string_value());
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMS, std::move(query));
    }
    else // get messages
    {
        DBServiceAPI::SMSGetLimitOffset(ownerServicePtr, 0, getJSON[json::messages::count].int_value());
    }

    return sys::ReturnCodes::Success;
}

auto MessageHelper::requestTemplate(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes
{
    if (getJSON[json::messages::count].bool_value() == true) // get templates count
    {
        DBServiceAPI::SMSTemplateGetCount(ownerServicePtr);
    }
    else if (getJSON[json::messages::id].int_value() != 0) { // templates search
        auto query = std::make_unique<db::query::SMSTemplateGetByID>(getJSON[json::messages::id].int_value());
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));
    }
    else // get messages
    {
        DBServiceAPI::SMSTemplateGetLimitOffset(ownerServicePtr, 0, getJSON[json::messages::count].int_value());
    }

    return sys::ReturnCodes::Success;
}

auto MessageHelper::createDBEntry(json11::Json createJSON, uint32_t uuid) -> sys::ReturnCodes
{
    uuidQueue.push(uuid);

    if (createJSON[json::messages::msgTemplate].bool_value() == true) {
        return createTemplate(createJSON, uuid);
    }
    else {
        return createSMS(createJSON, uuid);
    }
}

auto MessageHelper::createTemplate(json11::Json createJSON, uint32_t uuid) -> sys::ReturnCodes
{
    SMSTemplateRecord record = from_json(createJSON);

    DBServiceAPI::SMSTemplateAdd(ownerServicePtr, record);

    return sys::ReturnCodes::Success;
}

auto MessageHelper::createSMS(json11::Json createJSON, uint32_t uuid) -> sys::ReturnCodes
{
    return sys::ReturnCodes::Success;
}

auto MessageHelper::deleteDBEntry(json11::Json deleteJSON, uint32_t uuid) -> sys::ReturnCodes
{
    uuidQueue.push(uuid);

    if (deleteJSON[json::messages::msgTemplate].bool_value() == true) {
        return deleteTemplate(deleteJSON, uuid);
    }
    else {
        return deleteSMS(deleteJSON, uuid);
    }
}

auto MessageHelper::deleteSMS(json11::Json deleteJSON, uint32_t uuid) -> sys::ReturnCodes
{
    auto record = std::make_unique<SMSRecord>();
    record->ID  = deleteJSON[json::messages::id].int_value();
    DBServiceAPI::SMSRemove(ownerServicePtr, *record);

    return sys::ReturnCodes::Success;
}

auto MessageHelper::deleteTemplate(json11::Json deleteJSON, uint32_t uuid) -> sys::ReturnCodes
{
    DBServiceAPI::SMSTemplateRemove(ownerServicePtr, deleteJSON[json::messages::id].int_value());
    return sys::ReturnCodes::Success;
}

auto MessageHelper::updateDBEntry(json11::Json updateJSON, uint32_t uuid) -> sys::ReturnCodes
{
    uuidQueue.push(uuid);

    if (updateJSON[json::messages::msgTemplate].bool_value() == true) {
        return updateTemplate(updateJSON, uuid);
    }
    else {
        return updateSMS(updateJSON, uuid);
    }
}

auto MessageHelper::updateSMS(json11::Json updateJSON, uint32_t uuid) -> sys::ReturnCodes
{
    using namespace db::query::smsthread;

    auto query = std::make_unique<db::query::smsthread::MarkAsRead>(
        updateJSON[json::messages::threadID].int_value(),
        (updateJSON[json::messages::isUnread].bool_value() ? MarkAsRead::Read::False : MarkAsRead::Read::True));
    LOG_DEBUG("Marking SMS thread as %d...", updateJSON[json::messages::isUnread].bool_value());

    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSThread, std::move(query));
    return sys::ReturnCodes::Success;
}

auto MessageHelper::updateTemplate(json11::Json updateJSON, uint32_t uuid) -> sys::ReturnCodes
{
    SMSTemplateRecord record;
    record.ID   = updateJSON[json::messages::id].int_value();
    record.text = updateJSON[json::messages::templateText].string_value();

    DBServiceAPI::SMSTemplateUpdate(ownerServicePtr, record);
    return sys::ReturnCodes::Success;
}

auto MessageHelper::processQueryResult(db::QueryResult *result) -> json11::Json
{
    if (dynamic_cast<db::query::SMSGetByIDResult *>(result) != nullptr) {
        auto SMSresult  = dynamic_cast<db::query::SMSGetByIDResult *>(result);
        auto recordJSON = to_json(SMSresult->getResults());
        return recordJSON;
    }

    else if (dynamic_cast<db::query::SMSGetByContactIDResult *>(result) != nullptr) {
        auto SMSresult = dynamic_cast<db::query::SMSGetByContactIDResult *>(result);
        json11::Json::array SMSarray;

        for (auto record : SMSresult->getResults()) {

            SMSarray.emplace_back(to_json(record));
        }

        return SMSarray;
    }

    else if (dynamic_cast<db::query::SMSGetByTextResult *>(result) != nullptr) {
        auto SMSresult = dynamic_cast<db::query::SMSGetByTextResult *>(result);
        json11::Json::array SMSarray;

        for (auto record : SMSresult->getResults()) {

            SMSarray.emplace_back(to_json(record));
        }

        return SMSarray;
    }

    else if (dynamic_cast<db::query::smsthread::MarkAsReadResult *>(result) != nullptr) {
        auto SMSresult = dynamic_cast<db::query::smsthread::MarkAsReadResult *>(result);

        json11::Json responsePayloadJson = json11::Json::object{
            {json::endpoint, static_cast<int>(Endpoint::messages)},
            {json::status, static_cast<int>(SMSresult->getResult() ? http::Code::OK : http::Code::InternalServerError)},
            {json::uuid, std::to_string(getLastUUID())},
            {json::body, ""}};
        return responsePayloadJson;
    }

    else if (dynamic_cast<db::query::SMSTemplateGetByIDResult *>(result) != nullptr) {
        auto SMSTemplateResult = dynamic_cast<db::query::SMSTemplateGetByIDResult *>(result);
        return to_json(SMSTemplateResult->getResults());
    }
    else {
        return json11::Json();
    }
}
