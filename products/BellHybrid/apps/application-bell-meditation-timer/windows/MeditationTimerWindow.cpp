// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "MeditationTimerWindow.hpp"

#include <log/log.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    MeditationTimerWindow::MeditationTimerWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::MeditationTimerContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::meditationTimer), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void MeditationTimerWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        sideListView =
            new gui::SideListView(this, 0, 0, style::window_width, style::window_height, presenter->getProvider());
        presenter->loadTimerList();
        sideListView->rebuildList(listview::RebuildType::Full);
        setFocusItem(sideListView);
    }

    bool MeditationTimerWindow::onInput(const gui::InputEvent &inputEvent)
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
