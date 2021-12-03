// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/data/StyleCommon.hpp>
#include "windows/SessionPausedWindow.hpp"

namespace gui
{
    SessionPausedWindow::SessionPausedWindow(app::ApplicationCommon *app)
        : AppWindow(app, gui::window::session_paused::sessionPaused)
    {
        buildInterface();
    }

    bool SessionPausedWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF) || inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            application->returnToPreviousWindow();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void SessionPausedWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        if (icon == nullptr) {

            icon = new Icon(this, 0, 0, style::window_width, style::window_height, {}, {});
            icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
            icon->image->setMargins({0, itStyle::icon::imageTopMargin, 0, itStyle::icon::imageBottomMargin});
            icon->image->set(itStyle::icon::imagePause, ImageTypeSpecifier::W_G);
            icon->text->setFont(itStyle::text::font);
            icon->text->setRichText(utils::translate("common_paused"));
        }
    }
} // namespace gui
