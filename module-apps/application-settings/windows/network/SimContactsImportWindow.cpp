// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimContactsImportWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

namespace gui
{
    SimContactsImportWindow::SimContactsImportWindow(
        app::Application *app, std::unique_ptr<SimContactsImportWindowContract::Presenter> simImportPresenter)
        : AppWindow(app, gui::window::name::import_contacts), presenter(std::move(simImportPresenter))
    {
        presenter->attach(this);
        buildInterface();
    }

    void SimContactsImportWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        list->rebuildList();
    }

    void SimContactsImportWindow::onClose(CloseReason reason)
    {
        if (reason != CloseReason::PhoneLock) {
            presenter->clearProviderData();
        }
    }

    void SimContactsImportWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_network_import_contacts_from_sim_card"));

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(::style::strings::common::back));

        list = new ListView(this,
                            style::window::default_left_margin,
                            style::window::default_vertical_pos,
                            style::listview::body_width_with_scroll,
                            style::window::default_body_height,
                            presenter->getSimContactsProvider(),
                            listview::ScrollBarType::Fixed);

        setFocusItem(list);
    }
} // namespace gui
