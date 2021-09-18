// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsMainWindow.hpp"
#include <application-bell-background-sounds/ApplicationBellBackgroundSounds.hpp>
#include <ApplicationBellBackgroundSounds.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace gui
{
    BGSoundsMainWindow::BGSoundsMainWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bgSounds::BGSoundsMainWindowContract::Presenter> &&presenter)
        : AppWindow(app, gui::name::window::main_window), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    auto BGSoundsMainWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            application->switchWindow(gui::window::name::bgSoundsTimerSelect);
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace gui
