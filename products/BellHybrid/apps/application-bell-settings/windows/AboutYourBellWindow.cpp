// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AboutYourBellWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>
#include <i18n/i18n.hpp>
#include <models/AboutYourBellModel.hpp>

namespace gui
{
    namespace
    {
        static constexpr auto top_margin = 41U;
    }

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
                            style::window::default_body_width,
                            style::window::default_body_height,
                            presenter->getPagesProvider(),
                            listview::ScrollBarType::Fixed);
        list->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        list->rebuildList();
    }

    void AboutYourBellWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        setFocusItem(list);
    }
} // namespace gui
