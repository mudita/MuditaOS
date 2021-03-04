// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/DesktopMessages.hpp"
#include "parser/MessageHandler.hpp"

namespace sdesktop
{
    using namespace parserFSM;
    namespace developerMode
    {
        using namespace json::developerMode;

        AppFocusChangeEvent::AppFocusChangeEvent(std::string appName)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{cmd::focus, appName}});
        }

        ScreenlockCheckEvent::ScreenlockCheckEvent(bool isLocked)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{cmd::isLocked, isLocked}});
        }

        CellularStateInfoRequestEvent::CellularStateInfoRequestEvent(std::string stateStr)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{getInfo::cellularStateInfo, stateStr}});
        }
    } // namespace developerMode
    namespace bluetooth
    {
        BluetoothStatusRequestEvent::BluetoothStatusRequestEvent(int state)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::bluetooth);
            context.setResponseBody(json11::Json::object{{json::bluetooth::state, state}});
        }
        ScanStartedEvent::ScanStartedEvent()
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::bluetooth);
            context.setResponseBody(json11::Json::object{{json::bluetooth::scan, json::bluetooth::btOn}});
        }
        ScanStoppedEvent::ScanStoppedEvent()
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::bluetooth);
            context.setResponseBody(json11::Json::object{{json::bluetooth::scan, json::bluetooth::btOff}});
        }
    } // namespace bluetooth

} // namespace sdesktop
