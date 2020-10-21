// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ContactRecord.hpp" // for ContactRecord
#include "Context.hpp"
#include "DBHelper.hpp" // for DBHelper

#include "Common/Query.hpp"
#include "Service/Common.hpp" // for ReturnCodes
#include "Service/Service.hpp"
#include "json/json11.hpp" // for Json
#include <application-phonebook/models/PhonebookModel.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include <string> // for string, allocator

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
        const inline std::string count           = "count";
        const inline std::string primaryName     = "priName";
        const inline std::string alternativeName = "altName";
        const inline std::string address         = "address";
        const inline std::string id              = "id";
        const inline std::string numbers         = "numbers";
        const inline std::string isBlocked       = "blocked";
        const inline std::string isFavourite     = "favourite";

    } // namespace json::contacts
} // namespace parserFSM
