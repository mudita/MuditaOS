// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include <endpoints/DBHelper.hpp>

#include <Common/Query.hpp>
#include <SMSRecord.hpp>
#include <SMSTemplateRecord.hpp>
#include <Service/Common.hpp>
#include <Service/Service.hpp>
#include <ThreadRecord.hpp>
#include <json/json11.hpp>

#include <memory>
#include <string>

namespace sys
{
    class Service;
} // namespace sys

namespace parserFSM
{
    class Context;

    class MessageHelper final : public DBHelper
    {
      public:
        MessageHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr){};

        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;

        auto createTemplate(Context &context) -> sys::ReturnCodes;
        auto requestTemplate(Context &context) -> sys::ReturnCodes;
        auto updateTemplate(Context &context) -> sys::ReturnCodes;
        auto deleteTemplate(Context &context) -> sys::ReturnCodes;

        auto createSMS(Context &context) -> sys::ReturnCodes;
        auto requestSMS(Context &context) -> sys::ReturnCodes;
        auto updateSMS(Context &context) -> sys::ReturnCodes;
        auto deleteSMS(Context &context) -> sys::ReturnCodes;

        // auto createSimpleResponse(Context &context) -> std::string override;

        static auto to_json(SMSRecord record) -> json11::Json;
        static auto to_json(ThreadRecord record) -> json11::Json;
        static auto to_json(SMSTemplateRecord record) -> json11::Json;
        static auto from_json(json11::Json msgJson) -> SMSTemplateRecord;

      private:
        json11::Json receivedJson;
    };

    namespace messages
    {
        inline constexpr auto id           = "id";
        inline constexpr auto count        = "count";
        inline constexpr auto offset       = "offset";
        inline constexpr auto phoneNumber  = "phoneNumber";
        inline constexpr auto isUnread     = "unread";
        inline constexpr auto contactID    = "contactID";
        inline constexpr auto date         = "date";
        inline constexpr auto dateSent     = "dateSent";
        inline constexpr auto type         = "type";
        inline constexpr auto threadID     = "threadID";
        inline constexpr auto msgTemplate  = "template";
        inline constexpr auto templateText = "text";
        namespace thread
        {
            inline constexpr auto msgCount       = "msgCount";
            inline constexpr auto snippet        = "snippet";
            inline constexpr auto unreadMsgCount = "unreadMsgCount";

        } // namespace thread

    } // namespace messages
} // namespace parserFSM
