// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SessionEndedPresenter.hpp"

#include <service-appmgr/Controller.hpp>

namespace app::meditation
{
    SessionEndedPresenter::SessionEndedPresenter(app::ApplicationCommon *app) : app{app}
    {}

    void SessionEndedPresenter::activate()
    {
        app::manager::Controller::sendAction(app, app::manager::actions::Home);
    }
} // namespace app::meditation
