// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "NotifyGUIBedtimeReminderAction.hpp"

#include <service-appmgr/include/service-appmgr/ServiceApplicationManagerName.hpp>
#include <appmgr/messages/AlarmMessage.hpp>

namespace alarms
{
    NotifyGUIBedtimeReminderAction::NotifyGUIBedtimeReminderAction(sys::Service &service) : service{service}
    {}
    bool NotifyGUIBedtimeReminderAction::execute()
    {
        return service.bus.sendUnicast(std::make_shared<BedtimeNotification>(), service::name::appmgr);
    }
    bool NotifyGUIBedtimeReminderAction::turnOff()
    {
        return true;
    }
} // namespace alarms
