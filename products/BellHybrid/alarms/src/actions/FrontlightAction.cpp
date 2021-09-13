// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrontlightAction.hpp"

#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>

namespace alarms
{
    FrontlightAction::FrontlightAction(sys::Service &service) : service{service}
    {}

    bool FrontlightAction::execute()
    {
        service.bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::turnOn),
                                service::name::evt_manager);
        return true;
    }

    bool FrontlightAction::turnOff()
    {
        service.bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::turnOff),
            service::name::evt_manager);
        return true;
    }
} // namespace alarms
