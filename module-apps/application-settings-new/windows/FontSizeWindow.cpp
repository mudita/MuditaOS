// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FontSizeWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"

#include <module-utils/i18n/i18n.hpp>

namespace gui
{

    FontSizeWindow::FontSizeWindow(app::Application *app) : BaseSettingsWindow(app, gui::window::name::font_size)
    {
        setTitle(utils::localize.get("app_settings_display_font_size"));
    }

    auto FontSizeWindow::buildOptionsList() -> std::list<gui::Option>
    {
        return std::list<gui::Option>();
    }

} // namespace gui
