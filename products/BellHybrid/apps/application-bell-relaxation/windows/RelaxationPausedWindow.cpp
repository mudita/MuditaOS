// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationPausedWindow.hpp"
#include "data/RelaxationStyle.hpp"
#include <ApplicationBellRelaxation.hpp>

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <Text.hpp>
#include <i18n/i18n.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace gui
{
    RelaxationPausedWindow::RelaxationPausedWindow(app::ApplicationCommon *app)
        : AppWindow(app, gui::window::name::relaxationPaused)
    {
        buildInterface();
    }

    void RelaxationPausedWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        auto body = new gui::VBox(this, 0, 0, style::window_width, style::window_height);
        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));

        new gui::Image(body, "big_pause_W_G");

        auto text = new gui::Text(body, 0, 0, body->getWidth(), relaxationStyle::pause::textH);
        text->setFont(relaxationStyle::descriptionFont);
        text->setText(utils::translate("common_paused"));
        text->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        text->setVisible(true);
    }

    auto RelaxationPausedWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            application->returnToPreviousWindow();
            return true;
        }

        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            application->switchWindow(gui::name::window::main_window);
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace gui
