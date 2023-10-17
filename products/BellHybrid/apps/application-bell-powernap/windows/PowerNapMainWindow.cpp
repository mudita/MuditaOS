// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapMainWindow.hpp"
#include <Style.hpp>
#include <SideListView.hpp>
#include <gui/input/InputEvent.hpp>
namespace gui
{
    PowerNapMainWindow::PowerNapMainWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::powernap::PowerNapMainWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::main_window), windowPresenter{std::move(windowPresenter)}
    {
        this->windowPresenter->attach(this);
        buildInterface();
    }

    void PowerNapMainWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);

        sideListView = new gui::SideListView(
            this, 0, 0, style::window_width, style::window_height, windowPresenter->getNapTimeProvider());
        windowPresenter->loadNapTimeList();
        sideListView->rebuildList(listview::RebuildType::Full);
        setFocusItem(sideListView);
    }

    bool PowerNapMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sideListView->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            windowPresenter->activate();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
