// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsTurnOffWindow.hpp"

#include <application-bell-settings/ApplicationBellSettings.hpp>

namespace gui
{
    BellSettingsTurnOffWindow::BellSettingsTurnOffWindow(app::ApplicationCommon *app, std::string name)
        : AppWindow(app, std::move(name))
    {
        buildInterface();
    }

    void BellSettingsTurnOffWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsTurnOffWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);
    }
} /* namespace gui */
