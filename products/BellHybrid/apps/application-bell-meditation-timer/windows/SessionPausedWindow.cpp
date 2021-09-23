// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SessionPausedWindow.hpp"

#include "log.hpp"
#include <i18n/i18n.hpp>
#include <Font.hpp>

namespace gui
{
    SessionPausedWindow::SessionPausedWindow(app::ApplicationCommon *app)
        : IconTextWindow(
              app, gui::name::window::session_paused, std::make_unique<app::meditation::MeditationBasePresenter>(app))
    {}

    bool SessionPausedWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF) || inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            application->returnToPreviousWindow();
            return true;
        }

        return WithTimerWindow::onInput(inputEvent);
    }

    void SessionPausedWindow::onTimeout()
    {}

    std::string SessionPausedWindow::getText()
    {
        return utils::translate("app_meditation_bell_paused");
        // return "<text>Paused</text>";
    }

    std::string SessionPausedWindow::getImageName()
    {
        return spStyle::icon::imageSource;
    }

    std::chrono::seconds SessionPausedWindow::getTimeout() const
    {
        return std::chrono::seconds::zero();
    }
} // namespace gui
