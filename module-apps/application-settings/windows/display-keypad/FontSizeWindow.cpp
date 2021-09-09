// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FontSizeWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

namespace gui
{

    FontSizeWindow::FontSizeWindow(app::ApplicationCommon *app) : BaseSettingsWindow(app, gui::window::name::font_size)
    {
        setTitle(utils::translate("app_settings_display_font_size"));
    }

    auto FontSizeWindow::buildOptionsList() -> std::list<gui::Option>
    {
        return std::list<gui::Option>();
    }

} // namespace gui
