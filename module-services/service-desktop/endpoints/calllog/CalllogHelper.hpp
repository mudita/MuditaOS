// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Context.hpp>
#include <endpoints/DBHelper.hpp>

#include <CalllogRecord.hpp>
#include <Common/Query.hpp>
#include <Service/Common.hpp>
#include <Service/Service.hpp>
#include <application-phonebook/models/PhonebookModel.hpp>
#include <json11.hpp>

#include <string>

namespace sys
{
    class Service;
} // namespace sys

namespace parserFSM
{
    class Context;

    class CalllogHelper : public DBHelper
    {
      public:
        explicit CalllogHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr){};

        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;

        auto requestCount(Context &context) -> sys::ReturnCodes;
        static auto to_json(CalllogRecord record) -> json11::Json;
        auto getCalllogCount(Context &context) -> sys::ReturnCodes;
        auto getCalllogByContactID(Context &context) -> sys::ReturnCodes;
    };

    namespace json::calllog
    {
        inline constexpr auto count        = "count";
        inline constexpr auto limit        = "limit";
        inline constexpr auto offset       = "offset";
        inline constexpr auto presentation = "presentation";
        inline constexpr auto date         = "date";
        inline constexpr auto duration     = "duration";
        inline constexpr auto id           = "id";
        inline constexpr auto type         = "type";
        inline constexpr auto name         = "name";
        inline constexpr auto contactId    = "contactId";
        inline constexpr auto phoneNumber  = "phoneNumber";
        inline constexpr auto isRead       = "isRead";

    } // namespace json::calllog
} // namespace parserFSM
