#include "Contacts.hpp"
#include "Common/Query.hpp"
#include "ContactRecord.hpp"
#include "ParserUtils.hpp"
#include "PhoneNumber.hpp"
#include "Service/Common.hpp"
#include "api/DBServiceAPI.hpp"
#include "application-phonebook/models/NewContactModel.hpp"
#include "log/log.hpp"
#include "json/json11.hpp"
#include <memory>
#include <queries/phonebook/QueryContactGet.hpp>
#include <service-desktop/parser/EndpointHandler.hpp>
#include <string>
#include <type_traits>

using namespace ParserStateMachine;

auto ContactHelper::getLastUUID() -> uint32_t
{
    auto uuid = uuidQueue.front();
    uuidQueue.pop();
    return uuid;
}

auto ContactHelper::to_json(ContactRecord record) -> json11::Json
{
    auto numberArray = json11::Json::array();

    for (auto number : record.numbers) {
        numberArray.emplace_back(number.number.getEntered().c_str());
    }

    auto recordEntry = json11::Json::object{{json::contacts::primaryName, record.primaryName.c_str()},
                                            {json::contacts::alternativeName, record.alternativeName.c_str()},
                                            {json::contacts::address, record.address.c_str()},
                                            {json::contacts::id, static_cast<int>(record.ID)},
                                            {json::contacts::isBlocked, record.isOnBlocked()},
                                            {json::contacts::isFavourite, record.isOnFavourites()},
                                            {json::contacts::numbers, numberArray}};
    return recordEntry;
}

auto ContactHelper::from_json(json11::Json contactJSON) -> std::shared_ptr<ContactRecord>
{
    auto newRecord             = std::make_shared<ContactRecord>();
    newRecord->primaryName     = UTF8(contactJSON[json::contacts::primaryName].string_value());
    newRecord->alternativeName = UTF8(contactJSON[json::contacts::alternativeName].string_value());
    newRecord->address         = UTF8(contactJSON[json::contacts::address].string_value());

    newRecord->contactType = ContactType ::USER;

    for (auto num : contactJSON[json::contacts::numbers].array_items()) {
        utils::PhoneNumber phoneNumber(num.string_value());
        auto contactNum = ContactRecord::Number(phoneNumber.get(), phoneNumber.toE164(), ContactNumberType ::PAGER);
        newRecord->numbers.push_back(contactNum);
    }

    newRecord->addToBlocked(contactJSON[json::contacts::isBlocked].bool_value());
    newRecord->addToFavourites(contactJSON[json::contacts::isFavourite].bool_value());
    return newRecord;
}

auto ContactHelper::requestDataFromDB(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes
{
    auto limit = getJSON[json::contacts::count].int_value();
    DBServiceAPI::GetQuery(
        ownerServicePtr, db::Interface::Name::Contact, std::make_unique<db::query::ContactGet>(0, limit, ""));

    uuidQueue.push(uuid);

    return sys::ReturnCodes::Success;
}

auto ContactHelper::createDBEntry(json11::Json createJSON, uint32_t uuid) -> sys::ReturnCodes
{
    uuidQueue.push(uuid);

    LOG_DEBUG("Creating %s %s ...",
              createJSON[json::contacts::primaryName].string_value().c_str(),
              createJSON[json::contacts::alternativeName].string_value().c_str());

    auto newRecord = from_json(createJSON);
    if (newRecord->numbers.empty()) {
        LOG_ERROR("Empty number, not added!");
        return sys::ReturnCodes::Failure;
    }

    DBServiceAPI::ContactAdd(ownerServicePtr, *newRecord);

    return sys::ReturnCodes::Success;
}

auto ContactHelper::requestContactByID(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes
{
    uuidQueue.push(uuid);
    receivedJson    = getJSON;
    auto id         = getJSON[json::contacts::id].int_value();
    auto contactRec = DBServiceAPI::ContactGetByID(ownerServicePtr, id);
    return sys::ReturnCodes::Success;
}

auto ContactHelper::updateDBEntry(json11::Json updateJSON, uint32_t uuid) -> sys::ReturnCodes
{
    return sys::ReturnCodes::Success;
}

auto ContactHelper::updateContact(ContactRecord contact) -> sys::ReturnCodes
{

    LOG_DEBUG("updating contact with ID: %d", static_cast<int>(contact.ID));
    if (auto primaryName = receivedJson[json::contacts::primaryName].string_value(); primaryName.empty() == false) {
        contact.primaryName = UTF8(primaryName);
    }
    if (auto alternativeName = receivedJson[json::contacts::alternativeName].string_value();
        alternativeName.empty() == false) {
        contact.alternativeName = UTF8(alternativeName);
    }
    if (auto address = receivedJson[json::contacts::address].string_value(); address.empty() == false) {
        contact.address = UTF8(address);
    }
    if (receivedJson[json::contacts::numbers].array_items().size() > 0) {
        contact.numbers.clear();
    }
    for (auto num : receivedJson[json::contacts::numbers].array_items()) {
        utils::PhoneNumber phoneNumber(num.string_value());
        auto contactNum = ContactRecord::Number(phoneNumber.get(), phoneNumber.toE164(), ContactNumberType::CELL);
        contact.numbers.push_back(contactNum);
    }

    contact.addToBlocked(receivedJson[json::contacts::isBlocked].bool_value());
    contact.addToFavourites(receivedJson[json::contacts::isFavourite].bool_value());

    auto ret = DBServiceAPI::ContactUpdate(ownerServicePtr, contact);
    LOG_DEBUG("Update finished, ret: %d", ret);

    return ret ? sys::ReturnCodes::Success : sys::ReturnCodes::Failure;
}
auto ContactHelper::deleteDBEntry(json11::Json deleteJSON, uint32_t uuid) -> sys::ReturnCodes
{
    uuidQueue.push(uuid);

    auto id = deleteJSON[json::contacts::id].int_value();
    DBServiceAPI::ContactRemove(ownerServicePtr, id);
    return sys::ReturnCodes::Success;
}
auto ContactHelper::processQueryResult(db::QueryResult *result) -> json11::Json
{

    auto message = dynamic_cast<db::query::ContactGetResult *>(result);
    assert(message != nullptr);

    auto recordsPtr = std::make_unique<std::vector<ContactRecord>>(message->getRecords());
    auto records    = recordsPtr.get();

    json11::Json::array contactsArray;

    for (auto record : *records) {

        contactsArray.emplace_back(to_json(record));
    }

    json11::Json responsePayloadJson = json11::Json::object{
        {ParserStateMachine::json::endpoint, static_cast<int>(ParserStateMachine::Endpoint::contacts)},
        {ParserStateMachine::json::status, static_cast<int>(ParserStateMachine::http::Code::OK)},
        {ParserStateMachine::json::uuid, std::to_string(getLastUUID())},
        {ParserStateMachine::json::body, contactsArray}};

    LOG_DEBUG("%s", responsePayloadJson.dump().c_str());
    return responsePayloadJson;
}
