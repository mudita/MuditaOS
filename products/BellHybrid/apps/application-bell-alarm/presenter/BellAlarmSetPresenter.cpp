// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmSetPresenter.hpp"
#include "application-bell-main/ApplicationBellMain.hpp"

#include <Timers/TimerFactory.hpp>
#include <service-appmgr/Controller.hpp>

namespace app::bell_alarm
{
    BellAlarmSetPresenter::BellAlarmSetPresenter(app::ApplicationCommon *app,
                                                 std::shared_ptr<AbstractAlarmModel> alarmModel)
        : app{app}, alarmModel{std::move(alarmModel)}
    {}

    bool BellAlarmSetPresenter::isAlarmActive() const noexcept
    {
        return alarmModel->isActive();
    }

    time_t BellAlarmSetPresenter::getAlarmTime() const noexcept
    {
        return alarmModel->getAlarmTime();
    }

    void BellAlarmSetPresenter::activate()
    {
        app::manager::Controller::sendAction(
            app, app::manager::actions::Launch, std::make_unique<app::ApplicationLaunchData>(app::applicationBellName));
    }
} // namespace app::bell_alarm
