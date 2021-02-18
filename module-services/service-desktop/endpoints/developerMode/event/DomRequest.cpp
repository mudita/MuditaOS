// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-desktop/endpoints/developerMode/event/DomRequest.hpp>

namespace sdesktop::developerMode
{
    DomRequestEvent::DomRequestEvent(Event &p) : Event(p)
    {}

    void DomRequestEvent::setJson(json11::Json json)
    {
        context.setResponseStatus(parserFSM::http::Code::OK);
        context.setResponseBody(json11::Json::object{{"dom", json}});
    };
} // namespace sdesktop::developerMode
