// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/developerMode/event/ATRequest.hpp>
#include <endpoints/developerMode/DeveloperModeHelper.hpp>
#include <endpoints/JsonKeyNames.hpp>

namespace sdesktop::developerMode
{
    ATResponseEvent::ATResponseEvent(std::string command, std::chrono::milliseconds timeout)
        : command(std::move(command)), timeout(timeout)
    {
        context.setResponseStatus(endpoints::http::Code::InternalServerError);
        context.setResponseBody(json11::Json::object{{endpoints::json::developerMode::ATResponse, ""}});
    }

    void ATResponseEvent::setResponse(const std::vector<std::string> &response)
    {
        context.setResponseBody(json11::Json::object{{endpoints::json::developerMode::ATResponse, response}});
        context.setResponseStatus(endpoints::http::Code::OK);
    }

} // namespace sdesktop::developerMode
