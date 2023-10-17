// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellRelaxation.hpp"
#include "presenter/RelaxationEndedPresenter.hpp"

#include <Application.hpp>
#include <service-appmgr/Controller.hpp>

namespace app::relaxation
{
    RelaxationEndedPresenter::RelaxationEndedPresenter(app::ApplicationCommon *app) : app{app}
    {}

    void RelaxationEndedPresenter::activate()
    {
        app::manager::Controller::sendAction(
            app,
            app::manager::actions::Launch,
            std::make_unique<app::ApplicationLaunchData>(app::applicationBellRelaxationName));
    }
} // namespace app::relaxation
