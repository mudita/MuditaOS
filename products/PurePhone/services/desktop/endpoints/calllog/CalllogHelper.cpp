// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/calllog/CalllogHelper.hpp>
#include <endpoints/message/Sender.hpp>

#include <BaseInterface.hpp>
#include <Common/Query.hpp>
#include <module-db/queries/calllog/QueryCalllogGet.hpp>
#include <module-db/queries/calllog/QueryCalllogGetByContactID.hpp>
#include <module-db/queries/calllog/QueryCalllogGetCount.hpp>
#include <module-db/queries/calllog/QueryCalllogRemove.hpp>
#include <service-db/DBServiceAPI.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace sdesktop::endpoints
{

    using sender::putToSendQueue;

    auto CalllogHelper::createDBEntry(Context &context) -> sys::ReturnCodes
    {
        return sys::ReturnCodes::Unresolved;
    }
    auto CalllogHelper::requestDataFromDB(Context &context) -> sys::ReturnCodes
    {
        if (context.getBody()[json::calllog::count] == true) {
            return getCalllogCount(context);
        }
        if (context.getBody()[json::calllog::contactId].int_value() != 0) {
            return getCalllogByContactID(context);
        }
        else {
            auto limit  = context.getBody()[json::calllog::limit].int_value();
            auto offset = context.getBody()[json::calllog::offset].int_value();
            auto query  = std::make_unique<db::query::CalllogGet>(limit, offset);

            auto listener = std::make_unique<db::EndpointListener>(
                [](db::QueryResult *result, Context context) {
                    if (auto contactResult = dynamic_cast<db::query::CalllogGetResult *>(result)) {

                        auto recordsPtr = std::make_unique<std::vector<CalllogRecord>>(contactResult->getRecords());
                        json11::Json::array calllogArray;

                        for (const auto &record : *recordsPtr) {
                            calllogArray.emplace_back(CalllogHelper::to_json(record));
                        }

                        context.setResponseBody(calllogArray);
                        putToSendQueue(context.createSimpleResponse());
                        return true;
                    }
                    return false;
                },
                context);

            query->setQueryListener(std::move(listener));
            DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Calllog, std::move(query));
            return sys::ReturnCodes::Success;
        }
    }

    auto CalllogHelper::getCalllogCount(Context &context) -> sys::ReturnCodes
    {
        auto query = std::make_unique<db::query::CalllogGetCount>(EntryState::ALL);

        auto listener = std::make_unique<db::EndpointListener>(
            [](db::QueryResult *result, Context context) {
                if (auto calllogResult = dynamic_cast<db::query::CalllogGetCountResult *>(result)) {

                    auto count = calllogResult->getCount();
                    context.setResponseBody(json11::Json::object({{json::calllog::count, static_cast<int>(count)}}));
                    putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                return false;
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Calllog, std::move(query));
        return sys::ReturnCodes::Success;
    }
    auto CalllogHelper::getCalllogByContactID(Context &context) -> sys::ReturnCodes
    {
        auto query =
            std::make_unique<db::query::CalllogGetByContactID>(context.getBody()[json::calllog::contactId].int_value());

        auto listener = std::make_unique<db::EndpointListener>(
            [](db::QueryResult *result, Context context) {
                if (auto calllogResult = dynamic_cast<db::query::CalllogGetByContactIDResult *>(result)) {

                    auto records = calllogResult->getResults();
                    json11::Json::array calllogArray;

                    for (const auto &record : records) {
                        calllogArray.emplace_back(CalllogHelper::to_json(record));
                    }

                    context.setResponseBody(calllogArray);
                    putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                return false;
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Calllog, std::move(query));
        return sys::ReturnCodes::Success;
    }

    auto CalllogHelper::updateDBEntry(Context &context) -> sys::ReturnCodes
    {
        return sys::ReturnCodes::Unresolved;
    }
    auto CalllogHelper::deleteDBEntry(Context &context) -> sys::ReturnCodes
    {
        auto query = std::make_unique<db::query::CalllogRemove>(context.getBody()[json::calllog::id].int_value());

        auto listener = std::make_unique<db::EndpointListener>(
            [](db::QueryResult *result, Context context) {
                if (auto calllogResult = dynamic_cast<db::query::CalllogRemoveResult *>(result)) {

                    context.setResponseStatus(calllogResult->getResults() ? http::Code::NoContent
                                                                          : http::Code::InternalServerError);
                    putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                return false;
            },
            context);
        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Calllog, std::move(query));
        return sys::ReturnCodes::Success;
    }
    auto CalllogHelper::requestCount(Context &context) -> sys::ReturnCodes
    {
        return sys::ReturnCodes::Unresolved;
    }
    auto CalllogHelper::to_json(CalllogRecord record) -> json11::Json
    {
        const auto ss = std::make_unique<std::stringstream>();

        (*ss) << record.date;
        std::string date = ss->str();

        ss->clear();
        ss->str(std::string{});

        (*ss) << record.duration;
        std::string duration = ss->str();

        auto recordEntry = json11::Json::object{{json::calllog::presentation, static_cast<int>(record.presentation)},
                                                {json::calllog::date, date},
                                                {json::calllog::duration, duration},
                                                {json::calllog::id, static_cast<int>(record.ID)},
                                                {json::calllog::type, static_cast<int>(record.type)},
                                                {json::calllog::phoneNumber, record.phoneNumber.getEntered()},
                                                {json::calllog::isRead, record.isRead}};
        return recordEntry;
    }

} // namespace sdesktop::endpoints
