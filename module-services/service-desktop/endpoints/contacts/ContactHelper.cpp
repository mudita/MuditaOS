#include "ContactRecord.hpp"
#include "ContactHelper.hpp"
#include "Common/Query.hpp"
#include "ParserUtils.hpp"
#include "Service/Common.hpp"
#include "api/DBServiceAPI.hpp"
#include "application-phonebook/models/NewContactModel.hpp"
#include "log/log.hpp"
#include "queries/phonebook/QueryContactGetByID.hpp"
#include "queries/phonebook/QueryContactUpdate.hpp"
#include "json/json11.hpp"
#include <queries/phonebook/QueryContactGet.hpp>
#include <queries/phonebook/QueryContactAdd.hpp>
#include <queries/phonebook/QueryContactRemove.hpp>

using namespace ParserStateMachine;

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

auto ContactHelper::from_json(json11::Json contactJSON) -> ContactRecord
{
    auto newRecord            = ContactRecord();
    newRecord.primaryName     = UTF8(contactJSON[json::contacts::primaryName].string_value());
    newRecord.ID              = contactJSON[json::contacts::id].int_value();
    newRecord.alternativeName = UTF8(contactJSON[json::contacts::alternativeName].string_value());
    newRecord.address         = UTF8(contactJSON[json::contacts::address].string_value());

    newRecord.contactType = ContactType ::USER;

    for (auto num : contactJSON[json::contacts::numbers].array_items()) {
        utils::PhoneNumber phoneNumber(num.string_value());
        auto contactNum = ContactRecord::Number(phoneNumber.get(), phoneNumber.toE164(), ContactNumberType ::CELL);
        newRecord.numbers.push_back(contactNum);
    }

    newRecord.addToBlocked(contactJSON[json::contacts::isBlocked].bool_value());
    newRecord.addToFavourites(contactJSON[json::contacts::isFavourite].bool_value());
    return newRecord;
}

auto ContactHelper::requestDataFromDB(Context &context) -> sys::ReturnCodes
{
    if (context.getBody()[json::contacts::id].int_value() != 0) {
        return requestContactByID(context);
    }

    auto limit = context.getBody()[json::contacts::count].int_value();
    auto query = std::make_unique<db::query::ContactGet>(0, limit, "");

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, uint32_t uuid) {
            if (auto contactResult = dynamic_cast<db::query::ContactGetResult *>(result)) {

                auto recordsPtr = std::make_unique<std::vector<ContactRecord>>(contactResult->getRecords());
                json11::Json::array contactsArray;

                for (auto record : *recordsPtr.get()) {
                    contactsArray.emplace_back(ContactHelper::to_json(record));
                }

                MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
                    true, static_cast<int>(EndpointType::contacts), uuid, contactsArray));
                return true;
            }
            else {
                return false;
            }
        },
        context.getUuid());

    query->setQueryListener(std::move(listener));

    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));

    return sys::ReturnCodes::Success;
}

auto ContactHelper::createDBEntry(Context &context) -> sys::ReturnCodes
{
    LOG_DEBUG("Creating %s %s ...",
              context.getBody()[json::contacts::primaryName].string_value().c_str(),
              context.getBody()[json::contacts::alternativeName].string_value().c_str());

    auto newRecord = from_json(context.getBody());
    if (newRecord.numbers.empty()) {
        LOG_ERROR("Empty number, not added!");
        return sys::ReturnCodes::Failure;
    }

    auto query = std::make_unique<db::query::ContactAdd>(newRecord);

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, uint32_t uuid) {
            if (auto contactResult = dynamic_cast<db::query::ContactAddResult *>(result)) {
                MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
                    contactResult->getResult(), static_cast<int>(EndpointType::contacts), uuid, json11::Json()));

                return true;
            }
            else {
                return false;
            }
        },
        context.getUuid());

    query->setQueryListener(std::move(listener));

    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));
    return sys::ReturnCodes::Success;
}

auto ContactHelper::requestContactByID(Context &context) -> sys::ReturnCodes
{
    auto id    = context.getBody()[json::contacts::id].int_value();
    auto query = std::make_unique<db::query::ContactGetByID>(id);

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, uint32_t uuid) {
            if (auto contactResult = dynamic_cast<db::query::ContactGetByIDResult *>(result)) {
                auto record = ContactHelper::to_json(contactResult->getResult());
                MessageHandler::putToSendQueue(
                    Endpoint::createSimpleResponse(true, static_cast<int>(EndpointType::contacts), uuid, record));

                return true;
            }
            else {
                return false;
            }
        },
        context.getUuid());

    query->setQueryListener(std::move(listener));

    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));
    return sys::ReturnCodes::Success;
}

auto ContactHelper::updateDBEntry(Context &context) -> sys::ReturnCodes
{
    auto contact = from_json(context.getBody());
    auto query   = std::make_unique<db::query::ContactUpdate>(contact);

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, uint32_t uuid) {
            if (auto contactResult = dynamic_cast<db::query::ContactUpdateResult *>(result)) {
                MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
                    contactResult->getResult(), static_cast<int>(EndpointType::contacts), uuid, json11::Json()));

                return true;
            }
            else {
                return false;
            }
        },
        context.getUuid());

    query->setQueryListener(std::move(listener));
    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));
    LOG_DEBUG("Query done");

    return sys::ReturnCodes::Success;
}

auto ContactHelper::deleteDBEntry(Context &context) -> sys::ReturnCodes
{
    auto id    = context.getBody()[json::contacts::id].int_value();
    auto query = std::make_unique<db::query::ContactRemove>(id);

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, uint32_t uuid) {
            if (auto contactResult = dynamic_cast<db::query::ContactRemoveResult *>(result)) {
                MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
                    contactResult->getResult(), static_cast<int>(EndpointType::contacts), uuid, json11::Json()));

                return true;
            }
            else {
                return false;
            }
        },
        context.getUuid());

    query->setQueryListener(std::move(listener));

    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));
    return sys::ReturnCodes::Success;
}
