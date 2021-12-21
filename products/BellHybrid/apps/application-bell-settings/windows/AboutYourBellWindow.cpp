// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AboutYourBellWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>
#include <i18n/i18n.hpp>
#include <models/AboutYourBellModel.hpp>

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
        navBar->setVisible(false);

        list = new ListView(this,
                            style::window::default_left_margin,
                            top_margin,
                            width,
                            height,
                            presenter->getPagesProvider(),
                            listview::ScrollBarType::Fixed);
        list->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        setFocusItem(list);
        list->rebuildList();
    }
} // namespace gui
