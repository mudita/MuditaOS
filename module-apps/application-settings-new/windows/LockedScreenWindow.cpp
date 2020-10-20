// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LockedScreenWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "i18/i18.hpp"

namespace gui
{

    LockedScreenWindow::LockedScreenWindow(app::Application *app) : BaseSettingsWindow(app, window::name::locked_screen)
    {
        buildInterface();
    }

    void LockedScreenWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_locked_screen"));
    }
} // namespace gui
