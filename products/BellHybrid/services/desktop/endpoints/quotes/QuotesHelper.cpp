// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <endpoints/quotes/QuotesHelper.hpp>
#include <endpoints/Context.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <Service/Service.hpp>
#include <service-db/DBServiceAPI.hpp>
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
        // TODO: https://appnroll.atlassian.net/browse/BH-2097
        return {Sent::No, ResponseContext{.status = http::Code::InternalServerError}};
    }

    auto QuotesHelper::processPut(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();
        if (const auto group = body[json::quotes::settings::group].string_value(); !group.empty()) {
            DBServiceAPI::QuotesGroupChanged(owner, group.c_str());
        }
        else if (const auto interval = body[json::quotes::settings::interval].string_value(); !interval.empty()) {
            // TODO: https://appnroll.atlassian.net/browse/BH-2095
        }
        else {
            return {Sent::No, ResponseContext{.status = http::Code::BadRequest}};
        }

        return {Sent::No, ResponseContext{.status = http::Code::OK}};
    }

    auto QuotesHelper::processDelete(Context &context) -> ProcessResult
    {
        // TODO: https://appnroll.atlassian.net/browse/BH-2097
        return {Sent::No, ResponseContext{.status = http::Code::InternalServerError}};
    }

} // namespace sdesktop::endpoints
