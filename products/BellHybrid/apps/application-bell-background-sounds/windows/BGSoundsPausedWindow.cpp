// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsPausedWindow.hpp"
#include "data/BGSoundsStyle.hpp"
#include <ApplicationBellBackgroundSounds.hpp>

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <Text.hpp>
#include <i18n/i18n.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace gui
{
    BGSoundsPausedWindow::BGSoundsPausedWindow(app::ApplicationCommon *app)
        : AppWindow(app, gui::window::name::bgSoundsPaused)
    {
        buildInterface();
    }

    void BGSoundsPausedWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        auto body = new gui::VBox(this, 0, 0, style::window_width, style::window_height);
        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));

        new gui::Image(body, "big_pause_W_G");

        auto text = new gui::Text(body, 0, 0, body->getWidth(), bgSoundsStyle::pause::textH);
        text->setFont(bgSoundsStyle::descriptionFont);
        text->setText(utils::translate("common_paused"));
        text->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        text->setVisible(true);
    }

    auto BGSoundsPausedWindow::onInput(const InputEvent &inputEvent) -> bool
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
