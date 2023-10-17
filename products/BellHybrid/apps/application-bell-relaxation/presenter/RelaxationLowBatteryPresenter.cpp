// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "presenter/RelaxationLowBatteryPresenter.hpp"

#include <Application.hpp>
#include <service-appmgr/Controller.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>

namespace app::relaxation
{
    RelaxationLowBatteryPresenter::RelaxationLowBatteryPresenter(app::ApplicationCommon *app) : app{app}
    {}

    void RelaxationLowBatteryPresenter::activate()
    {
        app::manager::Controller::sendAction(
            app, app::manager::actions::Launch, std::make_unique<app::ApplicationLaunchData>(app::applicationBellName));
    }
} // namespace app::relaxation
