// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FontSizeWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>

namespace gui
{

    FontSizeWindow::FontSizeWindow(app::Application *app) : BaseSettingsWindow(app, gui::window::name::font_size)
    {
        buildInterface();
    }

    void FontSizeWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_font_size"));
    }

} // namespace gui
