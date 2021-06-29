// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationSettings.hpp"
#include "FontSizeWindow.hpp"

#include <i18n/i18n.hpp>

namespace gui
{

    FontSizeWindow::FontSizeWindow(app::Application *app) : BaseSettingsWindow(app, gui::window::name::font_size)
    {
        setTitle(utils::translate("app_settings_display_font_size"));
    }

    auto FontSizeWindow::buildOptionsList() -> std::list<gui::Option>
    {
        return std::list<gui::Option>();
    }

} // namespace gui
