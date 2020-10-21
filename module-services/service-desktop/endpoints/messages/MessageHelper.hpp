// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBHelper.hpp" // for DBHelper
#include "Endpoint.hpp"
#include "SMSTemplateRecord.hpp" // for SMSTemplateRecord
#include "ThreadRecord.hpp"      // for ThreadRecord

#include "Common/Query.hpp"
#include "Service/Common.hpp" // for ReturnCodes
#include "Service/Service.hpp"
#include "json/json11.hpp" // for Json
#include <service-db/api/DBServiceAPI.hpp>
#include <memory>
#include <string> // for string, allocator

namespace sys
{
    class Service;
} // namespace sys
struct SMSRecord;

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
        const inline std::string id           = "id";
        const inline std::string count        = "count";
        const inline std::string offset       = "offset";
        const inline std::string phoneNumber  = "phoneNumber";
        const inline std::string isUnread     = "unread";
        const inline std::string contactID    = "contactID";
        const inline std::string date         = "date";
        const inline std::string dateSent     = "dateSent";
        const inline std::string type         = "type";
        const inline std::string threadID     = "threadID";
        const inline std::string msgTemplate  = "template";
        const inline std::string templateText = "text";
        namespace thread
        {
            const inline std::string msgCount       = "msgCount";
            const inline std::string snippet        = "snippet";
            const inline std::string unreadMsgCount = "unreadMsgCount";

        } // namespace thread

    } // namespace messages
} // namespace parserFSM
