#pragma once
#include "Context.hpp"
#include <module-utils/icalendarlib/icalendar.h>

namespace parserFSM
{
    class ContextICS : public Context
    {
        Event event;

        ContextICS(const std::string &ics) : Context()
        {
            event = new Event();
        }

        auto createSimpleICSResponse() -> std::string
        { /// TODO: change to ICS format (maybe not needed)
            /// EVENT structure from ICALENDAR LIB is most similar (maybe replace the std::string ics variable with this
            /// object)
            json11::Json responseJson = json11::Json::object{{json::endpoint, static_cast<int>(getEndpoint())},
                                                             {json::status, static_cast<int>(responseContext.status)},
                                                             {json::uuid, std::to_string(getUuid())},
                                                             {json::body, responseContext.ics}};
            return buildResponseStr(responseJson.dump().size(), responseJson.dump());
        }

        auto setResponseICS(const std::string &respBody)
        {
            responseContext.ics = respBody;
        }

        auto getICS() -> std::string
        {
            return ics;
        }
    };

} // namespace parserFSM
