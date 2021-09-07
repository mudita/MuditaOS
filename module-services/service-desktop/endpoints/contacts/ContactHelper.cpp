// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContactHelper.hpp"
#include <endpoints/Context.hpp>
#include <parser/MessageHandler.hpp>
#include <parser/ParserUtils.hpp>

#include <queries/phonebook/QueryContactGet.hpp>
#include <queries/phonebook/QueryContactAdd.hpp>
#include <queries/phonebook/QueryContactRemove.hpp>

#include <BaseInterface.hpp>
#include <Common/Common.hpp>
#include <Common/Query.hpp>
#include <ContactRecord.hpp>
#include <PhoneNumber.hpp>
#include <Service/Common.hpp>
#include <log.hpp>
#include <queries/RecordQuery.hpp>
#include <queries/phonebook/QueryContactGetByID.hpp>
#include <queries/phonebook/QueryContactUpdate.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <utf8/UTF8.hpp>

#include <memory>
#include <utility>
#include <vector>
#include <algorithm>

using namespace parserFSM;

auto ContactHelper::to_json(const ContactRecord &record) -> json11::Json
{
    auto numberArray = json11::Json::array();

    for (const auto &number : record.numbers) {
        numberArray.emplace_back(number.number.getEntered().c_str());
    }

    auto recordEntry = json11::Json::object{{json::contacts::primaryName, record.primaryName.c_str()},
                                            {json::contacts::alternativeName, record.alternativeName.c_str()},
                                            {json::contacts::address, record.address.c_str()},
                                            {json::contacts::mail, record.mail.c_str()},
                                            {json::contacts::note, record.note.c_str()},
                                            {json::contacts::id, static_cast<int>(record.ID)},
                                            {json::contacts::isBlocked, record.isOnBlocked()},
                                            {json::contacts::isFavourite, record.isOnFavourites()},
                                            {json::contacts::isICE, record.isOnIce()},
                                            {json::contacts::speedDial, record.speeddial.c_str()},
                                            {json::contacts::numbers, numberArray}};
    return recordEntry;
}

auto ContactHelper::from_json(const json11::Json &contactJSON) -> ContactRecord
{
    auto newRecord            = ContactRecord();
    newRecord.primaryName     = UTF8(contactJSON[json::contacts::primaryName].string_value());
    newRecord.ID              = contactJSON[json::contacts::id].int_value();
    newRecord.alternativeName = UTF8(contactJSON[json::contacts::alternativeName].string_value());
    newRecord.address         = UTF8(contactJSON[json::contacts::address].string_value());
    newRecord.mail            = UTF8(contactJSON[json::contacts::mail].string_value());
    newRecord.note            = UTF8(contactJSON[json::contacts::note].string_value());
    newRecord.speeddial       = UTF8(contactJSON[json::contacts::speedDial].string_value());

    for (const auto &num : contactJSON[json::contacts::numbers].array_items()) {
        utils::PhoneNumber phoneNumber(num.string_value());
        auto contactNum = ContactRecord::Number(phoneNumber.get(), phoneNumber.toE164(), ContactNumberType ::CELL);
        newRecord.numbers.push_back(contactNum);
    }

    newRecord.addToBlocked(contactJSON[json::contacts::isBlocked].bool_value());
    newRecord.addToFavourites(contactJSON[json::contacts::isFavourite].bool_value());
    newRecord.addToIce(contactJSON[json::contacts::isICE].bool_value());
    return newRecord;
}

auto ContactHelper::requestDataFromDB(Context &context) -> sys::ReturnCodes
{
    if (context.getBody()[json::contacts::id].int_value() != 0) {
        return requestContactByID(context);
    }
    else if (context.getBody()[json::contacts::count].bool_value()) {
        return requestCount(context);
    }

    try {
        auto &ctx          = dynamic_cast<PagedContext &>(context);
        std::size_t limit  = ctx.getBody()[json::contacts::limit].int_value();
        std::size_t offset = ctx.getBody()[json::contacts::offset].int_value();
        ctx.setRequestedLimit(limit);
        ctx.setRequestedOffset(offset);
        auto query = std::make_unique<db::query::ContactGetWithTotalCount>(std::min(ctx.getPageSize(), limit), offset);
        auto listener = std::make_unique<db::EndpointListenerWithPages>(
            [](db::QueryResult *result, PagedContext &context) {
                if (auto contactResult = dynamic_cast<db::query::ContactGetResultWithTotalCount *>(result)) {

                    auto recordsPtr = std::make_unique<std::vector<ContactRecord>>(contactResult->getRecords());
                    context.setTotalCount(contactResult->getAllLength());
                    json11::Json::array contactsArray;

                    for (const auto &record : *recordsPtr) {
                        contactsArray.emplace_back(ContactHelper::to_json(record));
                    }

                    context.setResponseBody(contactsArray);

                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            ctx);
        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));
    }
    catch (const std::exception &e) {
        LOG_ERROR("exception while requesting data from DB");
        return sys::ReturnCodes::Failure;
    }

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ContactHelper::requestCount(Context &context)
{
    auto query = std::make_unique<db::query::ContactGetSize>();

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, Context context) {
            if (auto contactResult = dynamic_cast<db::query::RecordsSizeQueryResult *>(result)) {

                auto count = contactResult->getSize();

                context.setResponseBody(json11::Json::object({{json::contacts::count, static_cast<int>(count)}}));
                MessageHandler::putToSendQueue(context.createSimpleResponse());
                return true;
            }
            else {
                return false;
            }
        },
        context);

    query->setQueryListener(std::move(listener));

    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));

    return sys::ReturnCodes::Success;
}

auto ContactHelper::createDBEntry(Context &context) -> sys::ReturnCodes
{
    auto newRecord = from_json(context.getBody());
    if (newRecord.numbers.empty()) {
        LOG_ERROR("Empty number, not added!");
        context.setResponseStatus(http::Code::NotAcceptable);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Failure;
    }

    auto query = std::make_unique<db::query::ContactAdd>(newRecord);

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, Context context) {
            if (auto contactResult = dynamic_cast<db::query::ContactAddResult *>(result)) {

                context.setResponseBody(
                    json11::Json::object({{json::contacts::id, static_cast<int>(contactResult->getID())}}));
                if (contactResult->getResult()) {
                    context.setResponseStatus(http::Code::OK);
                }
                else if (contactResult->isDuplicated()) {
                    context.setResponseStatus(http::Code::Conflict);
                }
                else {
                    context.setResponseStatus(http::Code::InternalServerError);
                }
                MessageHandler::putToSendQueue(context.createSimpleResponse());

                return true;
            }
            else {
                return false;
            }
        },
        context);

    query->setQueryListener(std::move(listener));

    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));
    return sys::ReturnCodes::Success;
}

auto ContactHelper::requestContactByID(Context &context) -> sys::ReturnCodes
{
    auto id    = context.getBody()[json::contacts::id].int_value();
    auto query = std::make_unique<db::query::ContactGetByID>(id);

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, Context context) {
            if (auto contactResult = dynamic_cast<db::query::ContactGetByIDResult *>(result)) {
                auto record = ContactHelper::to_json(contactResult->getResult());

                context.setResponseBody(record);
                MessageHandler::putToSendQueue(context.createSimpleResponse());

                return true;
            }
            else {
                return false;
            }
        },
        context);

    query->setQueryListener(std::move(listener));

    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));
    return sys::ReturnCodes::Success;
}

auto ContactHelper::updateDBEntry(Context &context) -> sys::ReturnCodes
{
    auto contact = from_json(context.getBody());
    auto query   = std::make_unique<db::query::ContactUpdate>(contact);

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, Context context) {
            if (auto contactResult = dynamic_cast<db::query::ContactUpdateResult *>(result)) {

                context.setResponseStatus(contactResult->getResult() ? http::Code::NoContent
                                                                     : http::Code::InternalServerError);
                MessageHandler::putToSendQueue(context.createSimpleResponse());

                return true;
            }
            else {
                return false;
            }
        },
        context);

    query->setQueryListener(std::move(listener));
    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));

    return sys::ReturnCodes::Success;
}

auto ContactHelper::deleteDBEntry(Context &context) -> sys::ReturnCodes
{
    auto id    = context.getBody()[json::contacts::id].int_value();
    auto query = std::make_unique<db::query::ContactRemove>(id);

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, Context context) {
            if (auto contactResult = dynamic_cast<db::query::ContactRemoveResult *>(result)) {

                context.setResponseStatus(contactResult->getResult() ? http::Code::NoContent
                                                                     : http::Code::InternalServerError);
                MessageHandler::putToSendQueue(context.createSimpleResponse());

                return true;
            }
            else {
                return false;
            }
        },
        context);

    query->setQueryListener(std::move(listener));

    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));
    return sys::ReturnCodes::Success;
}
