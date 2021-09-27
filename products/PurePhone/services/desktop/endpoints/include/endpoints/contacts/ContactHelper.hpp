// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/DBHelper.hpp>

#include <ContactRecord.hpp>
#include <system/Common.hpp>
#include <Service/Service.hpp>
#include <json11.hpp>

#include <string>

namespace sdesktop::endpoints
{
    class ContactHelper : public DBHelper
    {

      public:
        explicit ContactHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr)
        {}

        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;

        auto requestCount(Context &context) -> sys::ReturnCodes;
        auto requestContactByID(Context &context) -> sys::ReturnCodes;
        static auto to_json(const ContactRecord &record) -> json11::Json;
        static auto from_json(const json11::Json &contactJSON) -> ContactRecord;
    };

    namespace json::contacts
    {
        inline constexpr auto limit           = "limit";
        inline constexpr auto offset          = "offset";
        inline constexpr auto primaryName     = "priName";
        inline constexpr auto alternativeName = "altName";
        inline constexpr auto address         = "address";
        inline constexpr auto note            = "note";
        inline constexpr auto mail            = "email";
        inline constexpr auto id              = "id";
        inline constexpr auto numbers         = "numbers";
        inline constexpr auto isBlocked       = "blocked";
        inline constexpr auto isFavourite     = "favourite";
        inline constexpr auto isICE           = "ice";
        inline constexpr auto speedDial       = "speedDial";
        inline constexpr auto count           = "count";

    } // namespace json::contacts

} // namespace sdesktop::endpoints
