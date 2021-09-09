// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuoteCategoriesWindow.hpp"

#include <application-settings/widgets/SettingsStyle.hpp>
#include <application-settings/windows/WindowNames.hpp>

namespace gui
{
    QuoteCategoriesWindow::QuoteCategoriesWindow(app::ApplicationCommon *app)
        : AppWindow(app, gui::window::name::quote_categories),
          categoriesModel(std::make_shared<Quotes::CategoriesModel>(app))
    {
        buildInterface();
    }

    void QuoteCategoriesWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_display_wallpaper_quotes_categories"));

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::check));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        list = new gui::ListView(this,
                                 style::quotes::categories::list::X,
                                 style::quotes::categories::list::Y,
                                 style::quotes::categories::list::Width,
                                 style::quotes::categories::list::Height,
                                 categoriesModel,
                                 gui::listview::ScrollBarType::Fixed);

        setFocusItem(list);
    }

    void QuoteCategoriesWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            list->rebuildList();
        }
    }
} // namespace gui
