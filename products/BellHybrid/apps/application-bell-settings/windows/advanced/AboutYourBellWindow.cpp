// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AboutYourBellWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>
#include <i18n/i18n.hpp>
#include <models/advanced/AboutYourBellModel.hpp>

namespace gui
{
    AboutYourBellWindow::AboutYourBellWindow(
        app::ApplicationCommon *app,
        std::shared_ptr<app::bell_settings::AboutYourBellWindowContract::Presenter> technicalPresenter)
        : AppWindow(app, gui::window::name::technical_information), presenter(std::move(technicalPresenter))
    {
        presenter->attach(this);
        buildInterface();
    }

    void AboutYourBellWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        list = new ListView(this,
                            style::window::default_left_margin,
                            style::window::default_vertical_pos,
                            style::listview::body_width_with_scroll,
                            style::window::default_body_height,
                            presenter->getPagesProvider(),
                            listview::ScrollBarType::Fixed);

        setFocusItem(list);
        list->rebuildList();
    }
} // namespace gui
