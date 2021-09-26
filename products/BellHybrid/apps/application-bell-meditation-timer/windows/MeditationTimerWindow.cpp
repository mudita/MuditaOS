// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimerWindow.hpp"
#include "IntervalChimeWindow.hpp"

#include <log/log.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    MeditationTimerWindow::MeditationTimerWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::MeditationTimerContract::Presenter> &&windowPresenter)
        : MeditationWindow(app, gui::name::window::meditation_timer), presenter{std::move(windowPresenter)}
    {
        buildInterface();
        presenter->attach(this);
    }

    void MeditationTimerWindow::buildInterface()
    {
        MeditationWindow::buildInterface();

        sideListView =
            new gui::SideListView(this, 0, 0, style::window_width, style::window_height, presenter->getProvider());
        setFocusItem(sideListView);
    }

    void MeditationTimerWindow::destroyInterface()
    {
        erase();
        sideListView = nullptr;
    }

    void MeditationTimerWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        MeditationWindow::onBeforeShow(mode, data);
    }

    bool MeditationTimerWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sideListView->onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            gotoWindow(gui::name::window::interval_chime);
            return true;
        }

        return MeditationWindow::onInput(inputEvent);
    }

    status_bar::Configuration MeditationTimerWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(status_bar::Indicator::Time);
        return appConfiguration;
    }

    void MeditationTimerWindow::buildMeditationItem(MeditationItem &item)
    {
        presenter->get(item);
    }

    void MeditationTimerWindow::onMeditationItemAvailable(MeditationItem *item)
    {
        if (item == nullptr) {
            item = new MeditationItem();
        }
        presenter->set(*item);
        sideListView->rebuildList(listview::RebuildType::Full);
    }
} // namespace gui
