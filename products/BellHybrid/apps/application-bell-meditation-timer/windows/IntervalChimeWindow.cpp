// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalChimeWindow.hpp"
#include "ReadyGoingWindow.hpp"

#include <i18n/i18n.hpp>

namespace gui
{
    IntervalChimeWindow::IntervalChimeWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::IntervalChimeContract::Presenter> &&windowPresenter)
        : MeditationWindow(app, gui::name::window::interval_chime), presenter{std::move(windowPresenter)}
    {
        buildInterface();
        presenter->attach(this);
    }

    void IntervalChimeWindow::buildInterface()
    {
        MeditationWindow::buildInterface();

        sideListView =
            new gui::SideListView(this, 0, 0, style::window_width, style::window_height, presenter->getProvider());
        setFocusItem(sideListView);
    }

    void IntervalChimeWindow::destroyInterface()
    {
        erase();
        sideListView = nullptr;
    }

    void IntervalChimeWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        MeditationWindow::onBeforeShow(mode, data);
    }

    bool IntervalChimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sideListView->onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            gotoWindow(gui::name::window::ready_going);
            return true;
        }

        return MeditationWindow::onInput(inputEvent);
    }

    status_bar::Configuration IntervalChimeWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(status_bar::Indicator::Time);
        return appConfiguration;
    }

    void IntervalChimeWindow::buildMeditationItem(MeditationItem &item)
    {
        presenter->get(item);
    }

    void IntervalChimeWindow::onMeditationItemAvailable(MeditationItem *item)
    {
        if (item != nullptr) {
            presenter->set(*item);
        }
    }
} // namespace gui
