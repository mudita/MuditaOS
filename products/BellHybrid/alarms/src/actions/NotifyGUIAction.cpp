// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotifyGUIAction.hpp"

#include <apps-common/actions/AlarmTriggeredAction.hpp>
#include <service-appmgr/include/service-appmgr/ServiceApplicationManagerName.hpp>

namespace alarms
{
    NotifyGUIAction::NotifyGUIAction(sys::Service &service) : service{service}
    {}
    bool NotifyGUIAction::execute()
    {
        return service.bus.sendUnicast(std::make_shared<app::actions::AlarmTriggeredAction>(), service::name::appmgr);
    }
    bool NotifyGUIAction::turnOff()
    {
        return true;
    }
} // namespace alarms
