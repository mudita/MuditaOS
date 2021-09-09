// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsHomeViewWindow.hpp"

#include <application-bell-settings/ApplicationBellSettings.hpp>

namespace gui
{
    BellSettingsHomeViewWindow::BellSettingsHomeViewWindow(app::ApplicationCommon *app, std::string name)
        : AppWindow(app, std::move(name))
    {
        buildInterface();
    }

    void BellSettingsHomeViewWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsHomeViewWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);
    }
} /* namespace gui */
