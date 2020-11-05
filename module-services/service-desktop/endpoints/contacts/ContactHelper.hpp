// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-phonebook/models/PhonebookModel.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include <string> // for string, allocator

#include "Common/Query.hpp"
#include "Context.hpp"
#include "Service/Service.hpp"
#include "Service/Common.hpp" // for ReturnCodes
#include "DBHelper.hpp"       // for DBHelper
#include "ContactRecord.hpp"  // for ContactRecord
#include "json/json11.hpp"    // for Json

namespace sys
{
    class Service;
} // namespace sys

namespace parserFSM
{
    class Context;

    class ContactHelper : public DBHelper
    {
      public:
        ContactHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr){};

        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;

        auto requestCount(Context &context) -> sys::ReturnCodes;
        auto requestContactByID(Context &context) -> sys::ReturnCodes;
        static auto to_json(ContactRecord record) -> json11::Json;
        static auto from_json(json11::Json contactJSON) -> ContactRecord;
    };

    namespace json::contacts
    {
        inline constexpr auto count           = "count";
        inline constexpr auto primaryName     = "priName";
        inline constexpr auto alternativeName = "altName";
        inline constexpr auto address         = "address";
        inline constexpr auto id              = "id";
        inline constexpr auto numbers         = "numbers";
        inline constexpr auto isBlocked       = "blocked";
        inline constexpr auto isFavourite     = "favourite";

    } // namespace json::contacts
} // namespace parserFSM
