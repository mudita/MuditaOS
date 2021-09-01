// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsBedtimeToneWindow.hpp"

#include <application-bell-settings/ApplicationBellSettings.hpp>

namespace gui
{
    BellSettingsBedtimeToneWindow::BellSettingsBedtimeToneWindow(app::Application *app, std::string name)
        : AppWindow(app, std::move(name))
    {
        buildInterface();
    }

    void BellSettingsBedtimeToneWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsBedtimeToneWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);
    }
} /* namespace gui */
