// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsVolumeWindow.hpp"
#include <ApplicationBellBackgroundSounds.hpp>
#include <SideListView.hpp>

namespace gui
{
    BGSoundsVolumeWindow::BGSoundsVolumeWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bgSounds::BGSoundsVolumeContract::Presenter> &&presenter)
        : WindowWithTimer(app, gui::window::name::bgSoundsVolume), presenter{std::move(presenter)}
    {
        buildInterface();
        this->presenter->attach(this);
    }

    void BGSoundsVolumeWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        sideListView = new gui::SideListView(
            this, 0, 0, style::window_width, style::window_height, presenter->getVolumeProvider());
        presenter->loadVolumeData();
        sideListView->rebuildList(listview::RebuildType::Full);
        setFocusItem(sideListView);
    }

    bool BGSoundsVolumeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        resetTimer();
        if (sideListView->onInput(inputEvent)) {
            presenter->onVolumeChanged();
            return true;
        }
        return WindowWithTimer::onInput(inputEvent);
    }

} // namespace gui
