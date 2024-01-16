// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmSetPresenter.hpp"
#include "application-bell-main/ApplicationBellMain.hpp"
#include <service-appmgr/Controller.hpp>

namespace app::bell_alarm
{
    BellAlarmSetPresenter::BellAlarmSetPresenter(app::ApplicationCommon *app,
                                                 AbstractAlarmModel &alarmModel,
                                                 AbstractBatteryModel &batteryModel)
        : app{app}, alarmModel{alarmModel}, batteryModel{batteryModel}
    {}

    bool BellAlarmSetPresenter::isAlarmActive() const noexcept
    {
        return alarmModel.isActive();
    }

    time_t BellAlarmSetPresenter::getAlarmTime() const noexcept
    {
        return alarmModel.getAlarmTime();
    }

    void BellAlarmSetPresenter::activate()
    {
        app::manager::Controller::sendAction(
            app, app::manager::actions::Launch, std::make_unique<app::ApplicationLaunchData>(app::applicationBellName));
    }

    Layout BellAlarmSetPresenter::getLayout()
    {
        if (not isAlarmActive()) {
            layout = Layout::AlarmInactive;
        }
        else if (layout == Layout::Undefined) {
            const auto batteryStatus = batteryModel.getLevelState();
            const units::SOC soc     = batteryStatus.level;
            const auto state         = batteryStatus.state;
            if (not batteryModel.isBatteryCharging(state) && soc < constants::lowBatteryInfoThreshold) {
                layout = Layout::LowBatteryInfo;
            }
            else {
                layout = Layout::AlarmInfo;
            }
        }
        return layout;
    }

    void BellAlarmSetPresenter::lowBatteryInfoHandled()
    {
        layout = Layout::AlarmInfo;
    }
} // namespace app::bell_alarm
