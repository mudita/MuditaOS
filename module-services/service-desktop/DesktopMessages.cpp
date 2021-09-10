// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/DesktopMessages.hpp"
#include "parser/MessageHandler.hpp"
#include <module-bluetooth/Bluetooth/Device.hpp>
#include <endpoints/developerMode/DeveloperModeEndpoint.hpp>

namespace sdesktop
{
    using namespace endpoints;

    namespace developerMode
    {

        AppFocusChangeEvent::AppFocusChangeEvent(std::string appName)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{json::developerMode::focus, appName}});
        }

        ScreenlockCheckEvent::ScreenlockCheckEvent(bool isLocked)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{json::developerMode::phoneLocked, isLocked}});
        }

        CellularStateInfoRequestEvent::CellularStateInfoRequestEvent(std::string stateStr)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{json::developerMode::cellularStateInfo, stateStr}});
        }

        CellularSleepModeInfoRequestEvent::CellularSleepModeInfoRequestEvent(bool isInSleepMode)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{json::developerMode::cellularSleepModeInfo, isInSleepMode}});
        }
    } // namespace developerMode

    namespace usb
    {
        USBConfigured::USBConfigured(USBConfigurationType configurationType)
            : sys::DataMessage(MessageType::USBConfigured), configurationType(configurationType)
        {}

        USBConfigurationType USBConfigured::getConfigurationType() const noexcept
        {
            return configurationType;
        }
    } // namespace usb

} // namespace sdesktop
