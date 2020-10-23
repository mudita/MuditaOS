// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperWindow.hpp"

#include "CheckBoxWithLabel.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    WallpaperWindow::WallpaperWindow(app::Application *app) : BaseSettingsWindow(app, window::name::wallpaper)
    {
        buildInterface();
    }

    void WallpaperWindow::buildInterface()
    {
        setTitle(utils::localize.get("app_settings_display_locked_screen_wallpaper"));
        BaseSettingsWindow::buildInterface();

        auto body =
            new VBox(nullptr,
                     0,
                     title->offset_h() + style::margins::big,
                     this->getWidth(),
                     this->getHeight() - style::margins::big - this->title->offset_h() - bottomBar->getHeight());
        body->setEdges(gui::RectangleEdge::None);

        quotes = Option(
                     utils::localize.get("app_settings_display_wallpaper_select_quotes"),
                     [=](gui::Item &item) {
                         this->application->switchWindow(gui::window::name::quotes, gui::ShowMode::GUI_SHOW_INIT);
                         return true;
                     },
                     Arrow::Enabled)
                     .build();

        auto toggleBoxes = [this](CheckBoxWithLabel &active) {
            for (CheckBoxWithLabel *box : {logoBox, clockBox, quotesBox}) {
                box->setChecked(false);
                if (&active == box) {
                    box->setChecked(!box->isChecked());
                }
            }

            quotes->setVisible(quotesBox->isChecked());
        };

        logoBox =
            new CheckBoxWithLabel(body, 0, 0, utils::localize.get("app_settings_display_wallpaper_logo"), toggleBoxes);
        clockBox =
            new CheckBoxWithLabel(body, 0, 0, utils::localize.get("app_settings_display_wallpaper_clock"), toggleBoxes);
        quotesBox = new CheckBoxWithLabel(
            body, 0, 0, utils::localize.get("app_settings_display_wallpaper_quotes"), toggleBoxes);

        addWidget(body);
        body->addWidget(quotes);
        setFocusItem(body);

        quotes->setVisible(false);
    } // namespace gui

} // namespace gui
