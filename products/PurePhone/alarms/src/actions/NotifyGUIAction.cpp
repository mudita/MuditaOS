// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotifyGUIAction.hpp"
#include "log/log.hpp"

#include <apps-common/actions/AlarmTriggeredAction.hpp>
#include <service-appmgr/include/service-appmgr/Constants.hpp>
#include <service-appmgr/Controller.hpp>
#include <apps-common/popups/data/AlarmPopupRequestParams.hpp>

namespace alarms
{
    NotifyGUIAction::NotifyGUIAction(sys::Service &service) : service{service}
    {}

    bool NotifyGUIAction::execute(const AlarmEventRecord &record)
    {
        auto r = std::make_unique<AlarmEventRecord>(record);
        app::manager::Controller::sendAction(
            &service,
            app::manager::actions::ShowPopup,
            std::make_unique<gui::AlarmPopupRequestParams>(gui::AlarmPopupType::AlarmRing, std::move(r)));
        return true;
    }

    bool NotifyGUIAction::turnOff(const AlarmEventRecord & /*record*/)
    {
        return true;
    }
} // namespace alarms
