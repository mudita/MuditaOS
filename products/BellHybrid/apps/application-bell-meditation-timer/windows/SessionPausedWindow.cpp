// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SessionPausedWindow.hpp"
#include "../data/MeditationStyle.hpp"

#include "log.hpp"
#include <i18n/i18n.hpp>
#include <Font.hpp>

using namespace meditationStyle;

namespace gui
{
    SessionPausedWindow::SessionPausedWindow(app::Application *app) : AppWindow(app, gui::name::window::session_paused)
    {
        buildInterface();
    }

    void SessionPausedWindow::buildInterface()
    {
        AppWindow::buildInterface();

        appImage = new gui::Image(this, spStyle::icon::x, spStyle::icon::y, 0, 0, spStyle::icon::imageSource);

        text = new gui::Label(this, spStyle::text::x, spStyle::text::y, spStyle::text::w, spStyle::text::h);
        text->setFilled(false);
        text->setBorderColor(gui::ColorNoColor);
        text->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        text->setText("Paused");
    }

    void SessionPausedWindow::destroyInterface()
    {
        erase();
        appImage = nullptr;
        text     = nullptr;
    }

    void SessionPausedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);
    }

    bool SessionPausedWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF) || inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            application->returnToPreviousWindow();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
