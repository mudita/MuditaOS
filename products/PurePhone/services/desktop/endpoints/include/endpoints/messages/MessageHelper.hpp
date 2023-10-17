﻿// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/DBHelper.hpp>

#include <SMSRecord.hpp>
#include <SMSTemplateRecord.hpp>
#include <ThreadRecord.hpp>
#include <json11.hpp>

namespace sdesktop::endpoints
{

    class MessageHelper final : public DBHelper
    {
      public:
        explicit MessageHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr){};

        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;

        static auto toJson(const SMSRecord &record) -> json11::Json;
        static auto toJson(const ThreadRecord &thread) -> json11::Json;
        static auto toJson(const ThreadRecord &thread, const utils::PhoneNumber::View &number) -> json11::Json;
        static auto toJson(const SMSTemplateRecord &record) -> json11::Json;
        static auto fromJson(const json11::Json &msgJson) -> SMSTemplateRecord;

      private:
        auto requestThread(Context &context) -> sys::ReturnCodes;
        auto updateThread(Context &context) -> sys::ReturnCodes;
        auto deleteThread(Context &context) -> sys::ReturnCodes;

        auto requestSMS(Context &context) -> sys::ReturnCodes;
        auto createSMS(Context &context) -> sys::ReturnCodes;
        auto updateSMS(Context &context) -> sys::ReturnCodes;
        auto deleteSMS(Context &context) -> sys::ReturnCodes;

        auto requestTemplate(Context &context) -> sys::ReturnCodes;
        auto createTemplate(Context &context) -> sys::ReturnCodes;
        auto updateTemplate(Context &context) -> sys::ReturnCodes;
        auto deleteTemplate(Context &context) -> sys::ReturnCodes;

        void getThreadById(Context &context);
        auto getThreads(Context &context) -> sys::ReturnCodes;

        void requestCount(Context &context);
        void getMessageById(Context &context);
        auto getMessagesByThreadID(Context &context) -> sys::ReturnCodes;
        void getByMessageBody(Context &context);
        auto getMessages(Context &context) -> sys::ReturnCodes;

        void getTemplatesCount(Context &context);
        void getTemplateByID(Context &context);
        auto getMessagesTemplates(Context &context) -> sys::ReturnCodes;

        json11::Json receivedJson;
    };
} // namespace sdesktop::endpoints
