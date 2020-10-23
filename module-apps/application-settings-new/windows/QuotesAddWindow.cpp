// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesAddWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"

#include <i18/i18.hpp>
#include <widgets/Text.hpp>

namespace gui
{
    QuotesAddWindow::QuotesAddWindow(app::Application *app) : AppWindow(app, gui::window::name::quotes)
    {
        buildInterface();
    }

    void QuotesAddWindow::buildInterface()
    {
        AppWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_locked_screen_new_quote"));

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        auto text = new gui::Text(nullptr,
                                  style::window::default_left_margin,
                                  title->offset_h() + style::margins::big,
                                  style::window::default_body_width,
                                  style::window::default_body_height);

        addWidget(text);
        setFocusItem(text);
    }

} // namespace gui
