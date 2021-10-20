// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "IntervalChimeWindow.hpp"

#include <i18n/i18n.hpp>

namespace gui
{
    IntervalChimeWindow::IntervalChimeWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::IntervalChimeContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::intervalChime), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void IntervalChimeWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        sideListView =
            new gui::SideListView(this, 0, 0, style::window_width, style::window_height, presenter->getProvider());
        presenter->loadIntervalList();
        sideListView->rebuildList(listview::RebuildType::Full);
        setFocusItem(sideListView);
    }

    bool IntervalChimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sideListView->onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            presenter->activate();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }
} // namespace gui
