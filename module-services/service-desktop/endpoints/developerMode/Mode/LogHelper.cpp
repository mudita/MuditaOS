// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LogHelper.hpp"
#include "Logger.hpp"

namespace parserFSM
{
    auto LogHelper::processPost(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();

        if (body["service"].is_string() and body["level"].is_number()) {
            return setLogLevel(body["service"].string_value(), body["level"].number_value());
        }
        else if (body["device"].is_string()) {
            return setLogDevice(body["device"].string_value());
        }

        return {sent::no, std::nullopt};
    }

    auto LogHelper::processGet(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();
        if (body["service"].is_string()) {
            return getLogLevel(body["service"].string_value());
        }
        return {sent::no, std::nullopt};
    }

    BaseHelper::ProcessResult LogHelper::setLogLevel(const std::string &serviceName, double level)
    {
        if (Log::Logger::get().setLogLevel(serviceName, static_cast<logger_level>(level))) {
            return {sent::no, endpoint::ResponseContext{.status = http::Code::OK}};
        }
        else {
            return {sent::no, endpoint::ResponseContext{.status = http::Code::InternalServerError}};
        }
        return {sent::no, std::nullopt};
    }

    BaseHelper::ProcessResult LogHelper::setLogDevice(const std::string & /*device*/)
    {
        return {
            sent::no,
            endpoint::ResponseContext{http::Code::InternalServerError,
                                      std::map<std::string, std::string>{{"cause", "Logger is badly implemented"}}}};
    }

    BaseHelper::ProcessResult LogHelper::getLogLevel(const std::string &serviceName)
    {
        auto level = Log::Logger::get().getLogLevel(serviceName);
        return {sent::no, endpoint::ResponseContext{http::Code::OK, std::map<std::string, double>{{"level", level}}}};
    }

} // namespace parserFSM
