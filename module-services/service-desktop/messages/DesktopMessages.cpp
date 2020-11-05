// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DesktopMessages.hpp"

namespace sdesktop::developerMode
{
    void Event::send()
    {
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }

    ATResponseEvent::ATResponseEvent(std::vector<std::string> resp)
    {
        context.setResponseStatus(http::Code::OK);
        context.setEndpoint(EndpointType::developerMode);
        context.setResponseBody(json11::Json::object{{json::developerMode::ATResponse, resp}});
    }

    AppFocusChangeEvent::AppFocusChangeEvent(std::string appName)
    {
        context.setResponseStatus(http::Code::OK);
        context.setEndpoint(EndpointType::developerMode);
        context.setResponseBody(json11::Json::object{{json::developerMode::focus, appName}});
    }

    DeveloperModeRequest::DeveloperModeRequest(std::unique_ptr<Event> event)
        : sys::DataMessage(MessageType::DeveloperModeRequest), event(std::move(event))
    {}

    DeveloperModeRequest::DeveloperModeRequest() : sys::DataMessage(MessageType::DeveloperModeRequest)
    {}

} // namespace sdesktop::developerMode
