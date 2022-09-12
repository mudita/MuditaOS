// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsMainWindow.hpp"

#include <ListViewWithArrows.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include "common/options/BellOptionWindow.hpp"
#include "common/options/BellOptionsNavigation.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    BGSoundsMainWindow::BGSoundsMainWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bgSounds::BGSoundsMainWindowContract::Presenter> &&presenter)
        : AppWindow(app, gui::name::window::main_window), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }
    void BGSoundsMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        listView = new gui::ListViewWithArrows(
            this, 0, 0, style::window_width, style::window_height, presenter->getProvider());
        listView->applySizeRestrictions(style::bell_options_list::w,
                                        style::bell_options_list::h,
                                        style::bell_options_list::outer_layouts_h,
                                        style::bell_options_list::outer_layouts_margin);
        listView->setListTitle(utils::translate("app_bellmain_background_sounds"));

        auto storedCallback     = listView->inputCallback;
        listView->inputCallback = [&, storedCallback](Item &item, const InputEvent &event) {
            return storedCallback(item, invertNavigationDirection(event));
        };

        listView->rebuildList(listview::RebuildType::Full);

        setFocusItem(listView);
    }
} // namespace gui
