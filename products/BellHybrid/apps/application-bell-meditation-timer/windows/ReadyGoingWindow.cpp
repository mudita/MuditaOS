// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ReadyGoingWindow.hpp"
#include "MeditationRunningWindow.hpp"

namespace gui
{
    ReadyGoingWindow::ReadyGoingWindow(app::ApplicationCommon *app)
        : IconTextWindow(
              app, gui::name::window::ready_going, std::make_unique<app::meditation::MeditationBasePresenter>(app))
    {}

    void ReadyGoingWindow::onTimeout()
    {
        gotoWindow(gui::name::window::meditation_running);
    }

    std::string ReadyGoingWindow::getText()
    {
        return utils::translate("app_meditation_bell_put_down_and_wait");
    }

    std::string ReadyGoingWindow::getImageName()
    {
        return itStyle::icon::imageSource;
    }

    std::chrono::seconds ReadyGoingWindow::getTimeout() const
    {
        return timeout;
    }
} // namespace gui
