// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneNameWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "widgets/InputBox.hpp"

#include <Utils.hpp>

namespace gui
{
    PhoneNameWindow::PhoneNameWindow(app::Application *app) : AppWindow(app, gui::window::name::phone_name)
    {
        buildInterface();
    }

    void PhoneNameWindow::buildInterface()
    {
        AppWindow::buildInterface();
        topBar->setActive(TopBar::Elements::SIM, false);
        topBar->setActive(TopBar::Elements::TIME, true);

        setTitle(utils::localize.get("app_settings_bluetooth_phone_name"));

        inputField = inputBox(this, utils::localize.get("app_settings_bluetooth_phone_name"));
        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        setFocusItem(inputField);
    }

    void PhoneNameWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        inputField->clear();
        setFocusItem(inputField);
    }
} // namespace gui
