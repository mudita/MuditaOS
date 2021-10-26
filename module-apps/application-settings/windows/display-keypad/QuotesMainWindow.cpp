// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesMainWindow.hpp"

#include <application-settings/data/QuoteSwitchData.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <InputEvent.hpp>
#include <header/AddElementAction.hpp>

namespace style::quotes
{
    namespace list
    {
        constexpr auto X      = style::window::default_left_margin;
        constexpr auto Y      = style::window::default_vertical_pos;
        constexpr auto Width  = style::listview::body_width_with_scroll;
        constexpr auto Height = style::window_height - Y - style::footer::height;
    } // namespace list
} // namespace style::quotes

namespace gui
{
    QuotesMainWindow::QuotesMainWindow(app::ApplicationCommon *app)
        : AppWindow(app, gui::window::name::quotes), quotesModel(std::make_shared<Quotes::QuotesModel>(app))
    {
        buildInterface();
    }

    void QuotesMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_display_wallpaper_quotes"));
        header->navigationIndicatorAdd(new gui::header::AddElementAction(), gui::header::BoxSelection::Left);

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::check));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::options));

        list = new gui::ListView(this,
                                 style::quotes::list::X,
                                 style::quotes::list::Y,
                                 style::quotes::list::Width,
                                 style::quotes::list::Height,
                                 quotesModel,
                                 gui::listview::ScrollBarType::Fixed);

        setFocusItem(list);
    }

    auto QuotesMainWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_LEFT)) {
            application->switchWindow(gui::window::name::new_quote,
                                      std::make_unique<QuoteSwitchData>(QuoteAction::Add));
            return true;
        }

        return false;
    }

    void QuotesMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        list->rebuildList();
    }
} // namespace gui
