// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <endpoints/timeSync/TimeSyncHelper.hpp>
#include <endpoints/Context.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <service-time/ServiceTimeName.hpp>
#include <service-time/service-time/TimeMessage.hpp>
#include <Service/Service.hpp>
#include <log/log.hpp>

namespace sdesktop::endpoints
{
    auto TimeSyncHelper::processGet(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();

        /* Validate */
        if (body[json::timeSync::value] != json::timeSync::timestamp) {
            return {Sent::No, ResponseContext{.status = http::Code::BadRequest}};
        }

        std::time_t currentTimestamp;
        std::time(&currentTimestamp);

        json11::Json::object response({{json::timeSync::timestamp, currentTimestamp}});
        return {Sent::No, ResponseContext{.status = http::Code::OK, .body = std::move(response)}};
    }

    auto TimeSyncHelper::processPost(Context &context) -> ProcessResult
    {
        const auto &jsonTimestamp = context.getBody()[json::timeSync::timestamp];

        /* Validate */
        if (!jsonTimestamp.is_number()) {
            LOG_ERROR("Timestamp data type not a number!");
            return {Sent::No, ResponseContext{.status = http::Code::UnprocessableEntity}};
        }

        const auto timestamp = static_cast<std::time_t>(jsonTimestamp.number_value());
        logReceivedDateTime(timestamp);
        if (const auto success = sendTimeUpdateMessage(timestamp); !success) {
            LOG_ERROR("Failed to send time update message!");
            return {Sent::No, ResponseContext{.status = http::Code::InternalServerError}};
        }

        return {Sent::No, ResponseContext{.status = http::Code::OK}};
    }

    auto TimeSyncHelper::logReceivedDateTime(std::time_t timestamp) const noexcept -> void
    {
        const auto t = localtime(&timestamp);
        LOG_INFO("Received time set request: %d-%02d-%02d %02d:%02d:%02d",
                 t->tm_year + 1900,
                 t->tm_mon + 1,
                 t->tm_mday,
                 t->tm_hour,
                 t->tm_min,
                 t->tm_sec);
    }

    auto TimeSyncHelper::sendTimeUpdateMessage(std::time_t timestamp) const -> bool
    {
        auto msg = std::make_shared<stm::message::TimeChangeRequestMessage>(timestamp);
        return owner->bus.sendUnicast(std::move(msg), service::name::service_time);
    }
} // namespace sdesktop::endpoints
