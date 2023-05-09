// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/phoneNumber/PhoneNumberHelper.hpp>
#include <endpoints/message/Sender.hpp>

#include <queries/phonebook/QueryNumberGetByID.hpp>

#include <BaseInterface.hpp>
#include <Common/Query.hpp>
#include <PhoneNumber.hpp>
#include <system/Common.hpp>
#include <service-db/DBServiceAPI.hpp>

#include <memory>
#include <utility>
#include "queries/phonebook/QueryNumbersGetByIDs.hpp"

namespace sdesktop::endpoints
{

    using sender::putToSendQueue;

    auto PhoneNumberHelper::to_json(int numberID, const utils::PhoneNumber::View &phoneNumber) -> json11::Json
    {
        auto recordEntry = json11::Json::object{{json::phoneNumber::numberID, numberID},
                                                {json::phoneNumber::number, phoneNumber.getEntered().c_str()}};
        return recordEntry;
    }

    auto PhoneNumberHelper::requestDataFromDB(Context &context) -> sys::ReturnCodes
    {
        if (context.getBody()[json::phoneNumber::numberIDs].is_array()) {
            return requestNumberByNumberID(context);
        }
        return sys::ReturnCodes::Failure;
    }

    auto PhoneNumberHelper::requestNumberByNumberID(Context &context) -> sys::ReturnCodes
    {
        std::vector<std::uint32_t> numberIDs{};
        auto &jsonArray = context.getBody()[json::phoneNumber::numberIDs].array_items();
        std::transform(jsonArray.begin(), jsonArray.end(), std::back_inserter(numberIDs), [](const auto numberID) {
            return static_cast<std::uint32_t>(numberID.int_value());
        });
        auto query    = std::make_unique<db::query::NumbersGetByIDs>(numberIDs);
        auto listener = std::make_unique<db::EndpointListener>(
            [](db::QueryResult *result, Context context) {
                if (auto numbersResult = dynamic_cast<db::query::NumbersGetByIDsResult *>(result)) {
                    json11::Json::array numbersList{};
                    for (const auto &number : numbersResult->getNumbers()) {
                        numbersList.push_back(
                            PhoneNumberHelper::to_json(static_cast<int>(number.first), number.second));
                    }
                    context.setResponseBody(numbersList);
                    putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                return false;
            },
            context);

        query->setQueryListener(std::move(listener));

        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Contact, std::move(query));
        return sys::ReturnCodes::Success;
    }

    auto PhoneNumberHelper::createDBEntry(Context &context) -> sys::ReturnCodes
    {
        context.setResponseStatus(http::Code::BadRequest);
        return sys::ReturnCodes::Failure;
    };
    auto PhoneNumberHelper::updateDBEntry(Context &context) -> sys::ReturnCodes
    {
        context.setResponseStatus(http::Code::BadRequest);
        return sys::ReturnCodes::Failure;
    };
    auto PhoneNumberHelper::deleteDBEntry(Context &context) -> sys::ReturnCodes
    {
        context.setResponseStatus(http::Code::BadRequest);
        return sys::ReturnCodes::Failure;
    };

} // namespace sdesktop::endpoints
