// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/DBHelper.hpp>

#include <ContactRecord.hpp>
#include <system/Common.hpp>
#include <Service/Service.hpp>
#include <json11.hpp>

namespace sdesktop::endpoints
{
    class PhoneNumberHelper : public DBHelper
    {

      public:
        explicit PhoneNumberHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr)
        {}

        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;

        auto requestNumberByNumberID(Context &context) -> sys::ReturnCodes;

        static auto to_json(int numberID, const utils::PhoneNumber::View &phoneNumber) -> json11::Json;
    };

    namespace json::phoneNumber
    {
        inline constexpr auto numberIDs = "numberIDs";
        inline constexpr auto numberID  = "numberID";
        inline constexpr auto number    = "number";

    } // namespace json::phoneNumber

} // namespace sdesktop::endpoints
