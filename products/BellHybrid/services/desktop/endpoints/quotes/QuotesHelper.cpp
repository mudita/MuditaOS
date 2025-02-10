// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <endpoints/quotes/QuotesHelper.hpp>
#include <endpoints/Context.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <endpoints/message/Sender.hpp>
#include <Service/Service.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/QuotesMessages.hpp>
#include <log/log.hpp>

namespace sdesktop::endpoints
{
    auto QuotesHelper::processGet(Context &context) -> ProcessResult
    {
        // TODO: https://appnroll.atlassian.net/browse/BH-2099
        return {Sent::No, ResponseContext{.status = http::Code::BadRequest}};
    }

    auto QuotesHelper::processPost(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();
        if (const auto &quote = body[json::quotes::quote].string_value(); !quote.empty()) {
            const auto &author = body[json::quotes::author].string_value();

            auto listener = std::make_unique<db::EndpointListener>(
                [=](db::QueryResult *result, Context &context) {
                    const auto addQuoteResult = dynamic_cast<Quotes::Messages::AddQuoteResponse *>(result);
                    if (addQuoteResult == nullptr || !addQuoteResult->success) {
                        context.setResponseStatus(http::Code::InternalServerError);
                        sender::putToSendQueue(context.createSimpleResponse());
                        return false;
                    }

                    context.setResponseBody(
                        json11::Json::object{{json::quotes::quoteID, static_cast<int>(addQuoteResult->quoteId)}});
                    context.setResponseStatus(http::Code::OK);
                    sender::putToSendQueue(context.createSimpleResponse());
                    return true;
                },
                context);

            DBServiceAPI::QuotesAddNewEntry(owner, quote.c_str(), author.c_str(), std::move(listener));
            return {Sent::Yes, std::nullopt};
        }

        LOG_ERROR("Bad request! New quote is incorrect or missing!");
        return {Sent::No, ResponseContext{.status = http::Code::BadRequest}};
    }

    auto QuotesHelper::processPut(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();
        if (const auto &quote = body[json::quotes::quote].string_value(); !quote.empty()) {
            const auto &author = body[json::quotes::author].string_value();
            const auto id     = body[json::quotes::quoteID];

            if (!id.is_number()) {
                return {Sent::No, ResponseContext{.status = http::Code::BadRequest}};
            }

            auto listener = std::make_unique<db::EndpointListener>(
                [=](db::QueryResult *result, Context &context) {
                    const auto editQuoteResult = dynamic_cast<Quotes::Messages::EditEntryResponse *>(result);
                    if (editQuoteResult == nullptr || !editQuoteResult->success) {
                        context.setResponseStatus(http::Code::InternalServerError);
                        sender::putToSendQueue(context.createSimpleResponse());
                        return false;
                    }

                    context.setResponseStatus(http::Code::OK);
                    sender::putToSendQueue(context.createSimpleResponse());
                    return true;
                },
                context);

            DBServiceAPI::QuotesEditEntry(owner, id.int_value(), quote.c_str(), author.c_str(), std::move(listener));
            return {Sent::Yes, std::nullopt};
        }
        else if (const auto group = body[json::quotes::group].string_value(); !group.empty()) {
            DBServiceAPI::QuotesGroupChanged(owner, group.c_str());
        }
        else if (const auto &interval = body[json::quotes::interval].string_value(); !interval.empty()) {
            DBServiceAPI::QuotesIntervalChanged(owner, interval.c_str());
        }
        else {
            return {Sent::No, ResponseContext{.status = http::Code::BadRequest}};
        }

        return {Sent::No, ResponseContext{.status = http::Code::OK}};
    }

    auto QuotesHelper::processDelete(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();
        if (const auto quoteID = body[json::quotes::quoteID]; quoteID.is_number()) {
            auto listener = std::make_unique<db::EndpointListener>(
                [=](db::QueryResult *result, Context context) {
                    const auto deleteResult = dynamic_cast<Quotes::Messages::DeleteQuoteResponse *>(result);
                    if (deleteResult == nullptr || !deleteResult->success) {
                        context.setResponseStatus(http::Code::InternalServerError);
                        sender::putToSendQueue(context.createSimpleResponse());
                        return false;
                    }

                    context.setResponseStatus(http::Code::NoContent);
                    sender::putToSendQueue(context.createSimpleResponse());
                    return true;
                },
                context);

            DBServiceAPI::QuotesDeleteEntry(owner, quoteID.int_value(), std::move(listener));
            return {Sent::Yes, std::nullopt};
        }

        LOG_ERROR("Bad request! Quote ID is incorrect or missing!");
        return {Sent::No, ResponseContext{.status = http::Code::BadRequest}};
    }

} // namespace sdesktop::endpoints
