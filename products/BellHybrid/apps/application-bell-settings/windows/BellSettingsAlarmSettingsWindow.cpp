// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsAlarmSettingsWindow.hpp"

#include <application-bell-settings/ApplicationBellSettings.hpp>

namespace gui
{
    BellSettingsAlarmSettingsWindow::BellSettingsAlarmSettingsWindow(app::Application *app, std::string name)
        : OptionWindow(app, std::move(name))
    {
        buildInterface();
    }

    void BellSettingsAlarmSettingsWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsAlarmSettingsWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);
    }
} /* namespace gui */
