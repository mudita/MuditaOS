// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessageHelper.hpp"

#include <endpoints/Context.hpp>
#include <parser/MessageHandler.hpp>
#include <parser/ParserUtils.hpp>

#include <BaseInterface.hpp>
#include <Common/Query.hpp>
#include <SMSRecord.hpp>
#include <SMSTemplateRecord.hpp>
#include <Service/Common.hpp>
#include <ThreadRecord.hpp>
#include <queries/messages/sms/QuerySMSGet.hpp>
#include <queries/messages/sms/QuerySMSGetByID.hpp>
#include <queries/messages/sms/QuerySMSGetByThreadID.hpp>
#include <queries/messages/sms/QuerySMSGetCount.hpp>
#include <queries/messages/sms/QuerySMSRemove.hpp>
#include <queries/messages/templates/QuerySMSTemplateAdd.hpp>
#include <queries/messages/templates/QuerySMSTemplateGet.hpp>
#include <queries/messages/templates/QuerySMSTemplateGetByID.hpp>
#include <queries/messages/templates/QuerySMSTemplateGetCount.hpp>
#include <queries/messages/templates/QuerySMSTemplateRemove.hpp>
#include <queries/messages/templates/QuerySMSTemplateUpdate.hpp>
#include <queries/messages/threads/QueryThreadsGet.hpp>
#include <queries/messages/threads/QueryThreadMarkAsRead.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <utf8/UTF8.hpp>

#include <memory>
#include <utility>
#include <module-db/queries/messages/sms/QuerySMSGetByText.hpp>

namespace parserFSM
{

    auto MessageHelper::toJson(const SMSRecord &record) -> json11::Json
    {

        auto recordEntry = json11::Json::object{{json::messages::contactID, static_cast<int>(record.contactID)},
                                                {json::messages::createdAt, static_cast<int>(record.date)},
                                                {json::messages::messageID, static_cast<int>(record.ID)},
                                                {json::messages::messageBody, record.body.c_str()},
                                                {json::messages::messageType, static_cast<int>(record.type)},
                                                {json::messages::threadID, static_cast<int>(record.threadID)}};
        return recordEntry;
    }

    auto MessageHelper::toJson(const SMSTemplateRecord &record) -> json11::Json
    {

        auto recordEntry =
            json11::Json::object{{json::messages::templateID, static_cast<int>(record.ID)},
                                 {json::messages::templateBody, record.text.c_str()},
                                 {json::messages::lastUsedAt, static_cast<int>(record.lastUsageTimestamp)}};
        return recordEntry;
    }

    auto MessageHelper::toJson(const ThreadRecord &record) -> json11::Json
    {

        auto recordEntry = json11::Json::object{{json::messages::contactID, static_cast<int>(record.contactID)},
                                                {json::messages::numberID, static_cast<int>(record.numberID)},
                                                {json::messages::lastUpdatedAt, static_cast<int>(record.date)},
                                                {json::messages::messageCount, static_cast<int>(record.msgCount)},
                                                {json::messages::threadID, static_cast<int>(record.ID)},
                                                {json::messages::messageSnippet, record.snippet.c_str()},
                                                {json::messages::isUnread, record.isUnread()},
                                                {json::messages::messageType, static_cast<int>(record.type)}};
        return recordEntry;
    }

    auto MessageHelper::fromJson(const json11::Json &msgJson) -> SMSTemplateRecord
    {
        SMSTemplateRecord record;

        record.text = UTF8(msgJson[json::messages::templateBody].string_value());

        return record;
    }

    auto MessageHelper::requestDataFromDB(Context &context) -> sys::ReturnCodes
    {
        if (context.getBody()[json::messages::category].string_value() == json::messages::categoryMessage) {
            return requestSMS(context);
        }
        else if (context.getBody()[json::messages::category].string_value() == json::messages::categoryTemplate) {
            return requestTemplate(context);
        }
        else if (context.getBody()[json::messages::category].string_value() == json::messages::categoryThread) {
            return requestThread(context);
        }
        LOG_ERROR("Category of request is missing or incorrect!");
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Unresolved;
    }

    auto MessageHelper::createDBEntry(Context &context) -> sys::ReturnCodes
    {
        if (context.getBody()[json::messages::category].string_value() == json::messages::categoryMessage) {
            return createSMS(context);
        }
        else if (context.getBody()[json::messages::category].string_value() == json::messages::categoryTemplate) {
            return createTemplate(context);
        }
        LOG_ERROR("Category of request is missing or incorrect!");
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Unresolved;
    }

    auto MessageHelper::updateDBEntry(Context &context) -> sys::ReturnCodes
    {
        if (context.getBody()[json::messages::category].string_value() == json::messages::categoryTemplate) {
            return updateTemplate(context);
        }
        else if (context.getBody()[json::messages::category].string_value() == json::messages::categoryThread) {
            return updateThread(context);
        }
        LOG_ERROR("Category of request is missing or incorrect!");
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Unresolved;
    }

    auto MessageHelper::deleteDBEntry(Context &context) -> sys::ReturnCodes
    {
        if (context.getBody()[json::messages::category].string_value() == json::messages::categoryMessage) {
            return deleteSMS(context);
        }
        else if (context.getBody()[json::messages::category].string_value() == json::messages::categoryTemplate) {
            return deleteTemplate(context);
        }
        else if (context.getBody()[json::messages::category].string_value() == json::messages::categoryThread) {
            return deleteThread(context);
        }
        LOG_ERROR("Category of request is missing or incorrect!");
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Unresolved;
    }

    auto MessageHelper::requestSMS(Context &context) -> sys::ReturnCodes
    {
        if (context.getBody()[json::messages::count].bool_value()) {
            requestCount(context);
        }
        else if (context.getBody()[json::messages::messageID].int_value() != 0) {
            getMessageById(context);
        }
        else if (context.getBody()[json::messages::threadID].int_value() != 0) {
            return getMessagesByThreadID(context);
        }
        else if (!context.getBody()[json::messages::messageBody].string_value().empty()) {
            getByMessageBody(context);
        }
        else {
            return getMessages(context);
        }
        return sys::ReturnCodes::Success;
    }

    auto MessageHelper::createSMS(Context &context) -> sys::ReturnCodes
    {
        context.setResponseStatus(http::Code::InternalServerError);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Success;
    }

    auto MessageHelper::deleteSMS(Context &context) -> sys::ReturnCodes
    {
        if (!context.getBody()[json::messages::messageID].is_number()) {
            LOG_ERROR("Bad request! messageID is incorrect or missing!");
            return sys::ReturnCodes::Unresolved;
        }
        auto id       = context.getBody()[json::messages::messageID].int_value();
        auto query    = std::make_unique<db::query::SMSRemove>(id);
        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto smsTemplateResult = dynamic_cast<db::query::SMSRemoveResult *>(result)) {

                    context.setResponseStatus(smsTemplateResult->getResults() ? http::Code::OK
                                                                              : http::Code::InternalServerError);
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMS, std::move(query));

        return sys::ReturnCodes::Success;
    }

    auto MessageHelper::requestTemplate(Context &context) -> sys::ReturnCodes
    {
        if (context.getBody()[json::messages::count].bool_value()) {
            getTemplatesCount(context);
        }
        else if (context.getBody()[json::messages::templateID].int_value() != 0) {
            getTemplateByID(context);
        }
        else {
            getMessagesTemplates(context);
        }
        return sys::ReturnCodes::Success;
    }

    auto MessageHelper::updateTemplate(Context &context) -> sys::ReturnCodes
    {
        if (!context.getBody()[json::messages::templateID].is_number()) {
            LOG_ERROR("Bad request! templateID is incorrect or missing!");
            context.setResponseStatus(http::Code::BadRequest);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Unresolved;
        }

        if (!context.getBody()[json::messages::templateBody].is_string()) {
            LOG_ERROR("Bad request! templateBody is incorrect or missing!");
            context.setResponseStatus(http::Code::BadRequest);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Unresolved;
        }

        SMSTemplateRecord record;
        record.ID   = context.getBody()[json::messages::templateID].int_value();
        record.text = context.getBody()[json::messages::templateBody].string_value();

        auto query    = std::make_unique<db::query::SMSTemplateUpdate>(record);
        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto smsTemplateResult = dynamic_cast<db::query::SMSTemplateUpdateResult *>(result)) {

                    context.setResponseStatus(smsTemplateResult->getResult() ? http::Code::OK
                                                                             : http::Code::InternalServerError);
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));

        return sys::ReturnCodes::Success;
    }

    auto MessageHelper::createTemplate(Context &context) -> sys::ReturnCodes
    {
        if (!context.getBody()[json::messages::templateBody].is_string()) {
            LOG_ERROR("Bad request! templateBody is incorrect or missing!");
            context.setResponseStatus(http::Code::BadRequest);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Unresolved;
        }

        SMSTemplateRecord record = fromJson(context.getBody());

        auto query    = std::make_unique<db::query::SMSTemplateAdd>(record);
        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto smsTemplateResult = dynamic_cast<db::query::SMSTemplateAddResult *>(result)) {

                    context.setResponseStatus(smsTemplateResult->getResult() ? http::Code::OK
                                                                             : http::Code::InternalServerError);
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));

        return sys::ReturnCodes::Success;
    }

    auto MessageHelper::deleteTemplate(Context &context) -> sys::ReturnCodes
    {
        if (!context.getBody()[json::messages::templateID].is_number()) {
            LOG_ERROR("Bad request! templateID is incorrect or missing!");
            context.setResponseStatus(http::Code::BadRequest);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Unresolved;
        }

        auto query =
            std::make_unique<db::query::SMSTemplateRemove>(context.getBody()[json::messages::templateID].int_value());
        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto smsTemplateResult = dynamic_cast<db::query::SMSTemplateRemoveResult *>(result)) {

                    context.setResponseStatus(smsTemplateResult->getResults() ? http::Code::OK
                                                                              : http::Code::InternalServerError);
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));
        return sys::ReturnCodes::Success;
    }

    auto MessageHelper::requestThread(Context &context) -> sys::ReturnCodes
    {
        try {
            auto &ctx                = dynamic_cast<PagedContext &>(context);
            const std::size_t limit  = ctx.getBody()[json::messages::limit].int_value();
            const std::size_t offset = ctx.getBody()[json::messages::offset].int_value();
            ctx.setRequestedLimit(limit);
            ctx.setRequestedOffset(offset);
            auto query =
                std::make_unique<db::query::ThreadsGetWithTotalCount>(offset, std::min(ctx.getPageSize(), limit));

            auto listener = std::make_unique<db::EndpointListenerWithPages>(
                [](db::QueryResult *result, PagedContext &context) {
                    if (auto threadsResults = dynamic_cast<db::query::ThreadsGetResultsWithTotalCount *>(result)) {
                        json11::Json::array threadsArray;
                        auto theResults = threadsResults->getResults();
                        threadsArray.reserve(theResults.size());
                        for (auto &record : theResults) {
                            threadsArray.emplace_back(MessageHelper::toJson(std::move(record)));
                        }
                        context.setResponseBody(std::move(threadsArray));
                        context.setTotalCount(threadsResults->getTotalCount());
                        MessageHandler::putToSendQueue(context.createSimpleResponse());
                        return true;
                    }
                    else {
                        return false;
                    }
                },
                ctx);
            query->setQueryListener(std::move(listener));
            DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSThread, std::move(query));
        }
        catch (const std::bad_cast &e) {
            LOG_ERROR("%s", e.what());
            return sys::ReturnCodes::Failure;
        }
        return sys::ReturnCodes::Success;
    }

    auto MessageHelper::updateThread(Context &context) -> sys::ReturnCodes // Set thread as read/unread
    {
        if (!context.getBody()[json::messages::threadID].is_number()) {
            LOG_ERROR("Bad request! threadID is incorrect or missing!");
            context.setResponseStatus(http::Code::BadRequest);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Unresolved;
        }

        if (!context.getBody()[json::messages::isUnread].is_bool()) {
            LOG_ERROR("Bad request! isUnread is incorrect or missing!");
            context.setResponseStatus(http::Code::BadRequest);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Unresolved;
        }

        auto query = std::make_unique<db::query::MarkAsRead>(context.getBody()[json::messages::threadID].int_value(),
                                                             (context.getBody()[json::messages::isUnread].bool_value()
                                                                  ? db::query::MarkAsRead::Read::False
                                                                  : db::query::MarkAsRead::Read::True));

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto threadResult = dynamic_cast<db::query::MarkAsReadResult *>(result)) {

                    context.setResponseStatus(threadResult->getResult() ? http::Code::OK
                                                                        : http::Code::InternalServerError);
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));

        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSThread, std::move(query));

        return sys::ReturnCodes::Success;
    }

    auto MessageHelper::deleteThread(Context &context) -> sys::ReturnCodes
    {
        context.setResponseStatus(http::Code::InternalServerError);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Success;
    }

    void MessageHelper::requestCount(Context &context)
    {
        auto query = std::make_unique<db::query::SMSGetCount>();

        auto listener = std::make_unique<db::EndpointListener>(
            [](db::QueryResult *result, Context &context) {
                if (auto smsResult = dynamic_cast<db::query::SMSGetCountResult *>(result)) {
                    auto count = smsResult->getResults();
                    context.setResponseBody(json11::Json::object{{json::messages::count, static_cast<int>(count)}});
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMS, std::move(query));
    }

    void MessageHelper::getMessageById(Context &context)
    {
        auto query = std::make_unique<db::query::SMSGetByID>(context.getBody()[json::messages::messageID].int_value());

        auto listener = std::make_unique<db::EndpointListener>(
            [](db::QueryResult *result, Context &context) {
                if (auto smsResult = dynamic_cast<db::query::SMSGetByIDResult *>(result)) {

                    context.setResponseBody(MessageHelper::toJson(smsResult->getResults()));
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMS, std::move(query));
    }

    auto MessageHelper::getMessagesByThreadID(Context &context) -> sys::ReturnCodes
    {
        try {
            auto &ctx                = dynamic_cast<PagedContext &>(context);
            const std::size_t limit  = ctx.getBody()[json::messages::limit].int_value();
            const std::size_t offset = ctx.getBody()[json::messages::offset].int_value();
            ctx.setRequestedLimit(limit);
            ctx.setRequestedOffset(offset);
            auto query = std::make_unique<db::query::SMSGetByThreadIDWithTotalCount>(
                context.getBody()[json::messages::threadID].int_value(), std::min(ctx.getPageSize(), limit), offset);

            auto listener = std::make_unique<db::EndpointListenerWithPages>(
                [](db::QueryResult *result, PagedContext &context) {
                    if (auto smsResult = dynamic_cast<db::query::SMSGetByThreadIDResultWithTotalCount *>(result)) {

                        const auto &results = smsResult->getResults();
                        json11::Json::array smsArray;
                        smsArray.reserve(results.size());
                        for (const auto &record : results) {
                            smsArray.emplace_back(MessageHelper::toJson(record));
                        }

                        context.setResponseBody(std::move(smsArray));
                        context.setTotalCount(smsResult->getTotalCount());
                        MessageHandler::putToSendQueue(context.createSimpleResponse());
                        return true;
                    }
                    else {
                        return false;
                    }
                },
                ctx);

            query->setQueryListener(std::move(listener));
            DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMS, std::move(query));
        }
        catch (const std::bad_cast &e) {
            LOG_ERROR("%s", e.what());
            return sys::ReturnCodes::Failure;
        }
        return sys::ReturnCodes::Success;
    }

    void MessageHelper::getByMessageBody(Context &context)
    {
        // not adding pagination for this request, since it is just for development and testing purposes, and it's
        // not going to be used by Mudita Center
        auto query =
            std::make_unique<db::query::SMSGetByText>(context.getBody()[json::messages::messageBody].string_value());
        if (const auto filterByNumber = !context.getBody()[json::messages::phoneNumber].string_value().empty();
            filterByNumber) {
            utils::PhoneNumber number{context.getBody()[json::messages::phoneNumber].string_value()};
            query->filterByPhoneNumber(number.getView());
        }

        auto listener = std::make_unique<db::EndpointListener>(
            [](db::QueryResult *result, Context &context) {
                if (auto smsResult = dynamic_cast<db::query::SMSGetByTextResult *>(result)) {

                    const auto &results = smsResult->getResults();
                    json11::Json::array smsArray;
                    smsArray.reserve(results.size());
                    for (const auto &record : results) {
                        smsArray.emplace_back(MessageHelper::toJson(record));
                    }

                    context.setResponseBody(std::move(smsArray));
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMS, std::move(query));
    }

    auto MessageHelper::getMessages(Context &context) -> sys::ReturnCodes
    {
        try {
            auto &ctx                = dynamic_cast<PagedContext &>(context);
            const std::size_t limit  = ctx.getBody()[json::messages::limit].int_value();
            const std::size_t offset = ctx.getBody()[json::messages::offset].int_value();
            ctx.setRequestedLimit(limit);
            ctx.setRequestedOffset(offset);
            auto query = std::make_unique<db::query::SMSGetWithTotalCount>(std::min(ctx.getPageSize(), limit), offset);

            auto listener = std::make_unique<db::EndpointListenerWithPages>(
                [](db::QueryResult *result, PagedContext &context) {
                    if (auto smsResult = dynamic_cast<db::query::SMSGetResultWithTotalCount *>(result)) {

                        const auto &results = smsResult->getRecords();
                        json11::Json::array smsArray;
                        smsArray.reserve(results.size());
                        for (const auto &record : results) {
                            smsArray.emplace_back(MessageHelper::toJson(record));
                            LOG_DEBUG("Record found!: %" PRIu32 "\n", record.ID);
                        }

                        context.setResponseBody(std::move(smsArray));
                        context.setTotalCount(smsResult->getTotalCount());
                        MessageHandler::putToSendQueue(context.createSimpleResponse());
                        return true;
                    }
                    else {
                        return false;
                    }
                },
                ctx);

            query->setQueryListener(std::move(listener));
            DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMS, std::move(query));
        }
        catch (const std::bad_cast &e) {
            LOG_ERROR("%s", e.what());
            return sys::ReturnCodes::Failure;
        }
        return sys::ReturnCodes::Success;
    }

    void MessageHelper::getTemplatesCount(Context &context)
    {
        auto query = std::make_unique<db::query::SMSTemplateGetCount>();

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto smsResult = dynamic_cast<db::query::SMSTemplateGetCountResult *>(result)) {
                    auto count = smsResult->getResults();

                    context.setResponseBody(json11::Json::object{{json::messages::count, static_cast<int>(count)}});
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));
    }

    void MessageHelper::getTemplateByID(Context &context)
    {
        auto query =
            std::make_unique<db::query::SMSTemplateGetByID>(context.getBody()[json::messages::templateID].int_value());

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto smsTemplateResult = dynamic_cast<db::query::SMSTemplateGetByIDResult *>(result)) {

                    context.setResponseBody(MessageHelper::toJson(smsTemplateResult->getResults()));
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));
    }

    auto MessageHelper::getMessagesTemplates(Context &context) -> sys::ReturnCodes
    {
        try {
            auto &ctx                = dynamic_cast<PagedContext &>(context);
            const std::size_t limit  = ctx.getBody()[json::messages::limit].int_value();
            const std::size_t offset = ctx.getBody()[json::messages::offset].int_value();
            ctx.setRequestedLimit(limit);
            ctx.setRequestedOffset(offset);
            auto query =
                std::make_unique<db::query::SMSTemplateGetWithTotalCount>(std::min(ctx.getPageSize(), limit), offset);
            auto listener = std::make_unique<db::EndpointListenerWithPages>(
                [=](db::QueryResult *result, PagedContext &context) {
                    if (auto smsTemplateResult =
                            dynamic_cast<db::query::SMSTemplateGetResultWithTotalCount *>(result)) {
                        json11::Json::array smsTemplateArray;
                        const auto &results = smsTemplateResult->getResults();
                        smsTemplateArray.reserve(results.size());
                        for (const auto &record : results) {
                            smsTemplateArray.emplace_back(toJson(record));
                        }

                        context.setResponseBody(std::move(smsTemplateArray));
                        context.setTotalCount(smsTemplateResult->getTotalTemplatesCount());
                        MessageHandler::putToSendQueue(context.createSimpleResponse());
                        return true;
                    }
                    else {
                        return false;
                    }
                },
                ctx);

            query->setQueryListener(std::move(listener));
            DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));
        }
        catch (const std::bad_cast &e) {
            LOG_ERROR("%s", e.what());
            return sys::ReturnCodes::Failure;
        }
        return sys::ReturnCodes::Success;
    }
} // namespace parserFSM
