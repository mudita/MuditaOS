// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IgnoreKeysAction.hpp"

#include <service-evtmgr/ServiceEventManagerName.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>

namespace alarms
{
    IgnoreKeysAction::IgnoreKeysAction(sys::Service &service) : service{service}
    {}
    bool IgnoreKeysAction::execute()
    {
        return service.bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::ignoreKeypress),
            service::name::evt_manager);
    }
    bool IgnoreKeysAction::turnOff()
    {
        return true;
    }
} // namespace alarms
