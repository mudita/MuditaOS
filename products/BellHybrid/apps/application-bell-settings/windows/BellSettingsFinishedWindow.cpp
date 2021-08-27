// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsFinishedWindow.hpp"

#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>

namespace
{
    constexpr auto appNameToReturn = gui::window::name::bellSettingsAdvanced;
}

namespace gui
{

    BellSettingsFinishedWindow::BellSettingsFinishedWindow(app::Application *app, const std::string &name)
        : WindowWithTimer(app, name)
    {
        buildInterface();

        timerCallback = [this](Item &, sys::Timer &) {
            application->switchWindow(appNameToReturn);
            return true;
        };
    }
    void BellSettingsFinishedWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        icon = new Icon(this,
                        0,
                        0,
                        style::window_width,
                        style::window_height,
                        "circle_success",
                        utils::translate("app_bell_settings_time_units_finished_message"));
    }
    bool BellSettingsFinishedWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            application->switchWindow(appNameToReturn);
            return true;
        }
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }
    void BellSettingsFinishedWindow::rebuild()
    {
        erase();
        buildInterface();
    }

} // namespace gui
