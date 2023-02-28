// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "presenter/RelaxationErrorPresenter.hpp"
#include "ApplicationBellRelaxation.hpp"

#include <Application.hpp>
#include <service-appmgr/Controller.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>

namespace app::relaxation
{
    RelaxationErrorPresenter::RelaxationErrorPresenter(app::ApplicationCommon *app) : app{app}
    {}

    void RelaxationErrorPresenter::activate()
    {
        app::manager::Controller::sendAction(
            app,
            app::manager::actions::Launch,
            std::make_unique<app::ApplicationLaunchData>(app::applicationBellRelaxationName));
    }
} // namespace app::relaxation
