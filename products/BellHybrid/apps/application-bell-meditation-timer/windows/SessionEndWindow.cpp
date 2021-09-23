// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SessionEndWindow.hpp"

#include "log.hpp"
#include <service-appmgr/Controller.hpp>

namespace gui
{
    SessionEndWindow::SessionEndWindow(app::ApplicationCommon *app)
        : IconTextWindow(
              app, gui::name::window::session_end, std::make_unique<app::meditation::MeditationBasePresenter>(app))
    {}

    void SessionEndWindow::onTimeout()
    {
        app::manager::Controller::sendAction(application, app::manager::actions::Home);
    }

    std::string SessionEndWindow::getText()
    {
        return utils::translate("app_meditation_bell_thank_you_for_session");
    }

    std::string SessionEndWindow::getImageName()
    {
        return itStyle::icon::imageSource;
    }

    std::chrono::seconds SessionEndWindow::getTimeout() const
    {
        return timeout;
    }
} // namespace gui
