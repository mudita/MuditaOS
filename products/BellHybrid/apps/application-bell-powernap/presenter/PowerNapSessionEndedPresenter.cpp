// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapSessionEndedPresenter.hpp"
#include <Timers/TimerFactory.hpp>
#include <Application.hpp>
#include <service-appmgr/Controller.hpp>
#include "application-bell-main/ApplicationBellMain.hpp"

namespace app::powernap
{
    PowerNapSessionEndPresenter::PowerNapSessionEndPresenter(app::ApplicationCommon *app, AbstractAlarmModel &alarm)
        : app{app}, alarmModel{alarm}
    {}

    void PowerNapSessionEndPresenter::activate()
    {
        app::manager::Controller::sendAction(
            app, app::manager::actions::Launch, std::make_unique<app::ApplicationLaunchData>(app::applicationBellName));
    }
    bool PowerNapSessionEndPresenter::handleIfPreWakeupIsToTurnOffFirst()
    {
        if (alarmModel.isPreWakeUpActive()) {
            alarmModel.turnOffPreWakeUp();
            return true;
        }
        return false;
    }
} // namespace app::powernap
