// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "presenter/RelaxationErrorPresenter.hpp"
#include "ApplicationBellRelaxation.hpp"

#include <service-appmgr/Controller.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>

namespace app::relaxation
{
    RelaxationErrorPresenter::RelaxationErrorPresenter(app::ApplicationCommon *app, AbstractAlarmModel &alarm)
        : app{app}, alarmModel{alarm}
    {}

    void RelaxationErrorPresenter::activate()
    {
        app::manager::Controller::sendAction(
            app,
            app::manager::actions::Launch,
            std::make_unique<app::ApplicationLaunchData>(app::applicationBellRelaxationName));
    }

    bool RelaxationErrorPresenter::handleIfPreWakeupIsToTurnOffFirst()
    {
        if (alarmModel.isPreWakeUpActive()) {
            alarmModel.turnOffPreWakeUpFrontLight();
            return true;
        }
        return false;
    }
} // namespace app::relaxation
