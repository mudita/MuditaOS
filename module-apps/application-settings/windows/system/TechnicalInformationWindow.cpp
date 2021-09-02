// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TechnicalInformationWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

namespace gui
{
    TechnicalInformationWindow::TechnicalInformationWindow(
        app::Application *app, std::shared_ptr<TechnicalWindowContract::Presenter> technicalPresenter)
        : AppWindow(app, gui::window::name::technical_information), presenter(std::move(technicalPresenter))
    {
        presenter->attach(this);
        buildInterface();
    }

    void TechnicalInformationWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        presenter->requestImei();
    }

    void TechnicalInformationWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_about_your_pure"));
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(::style::strings::common::back));

        list = new ListView(this,
                            style::window::default_left_margin,
                            style::window::default_vertical_pos,
                            style::listview::body_width_with_scroll,
                            style::window::default_body_height,
                            presenter->getTechnicalInformationProvider(),
                            listview::ScrollBarType::Fixed);

        setFocusItem(list);
        list->rebuildList();
    }

    void TechnicalInformationWindow::imeiReady() noexcept
    {
        list->rebuildList();
    }

} // namespace gui
