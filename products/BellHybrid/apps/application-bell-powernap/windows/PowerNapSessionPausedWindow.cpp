// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapSessionPausedWindow.hpp"
#include <application-bell-powernap/ApplicationBellPowerNap.hpp>
#include <keymap/KeyMap.hpp>

namespace gui
{
    PowerNapSessionPausedWindow::PowerNapSessionPausedWindow(
        app::ApplicationCommon *app, std::shared_ptr<app::powernap::PowerNapSessionPausedContract::Presenter> presenter)
        : SessionPausedWindow(app, window::name::powerNapSessionPaused), presenter{std::move(presenter)}
    {}

    bool PowerNapSessionPausedWindow::onInput(const InputEvent &inputEvent)
    {
        const auto key = mapKey(inputEvent.getKeyCode());
        if (inputEvent.isShortRelease() && key != KeyMap::Frontlight &&
            presenter->handleIfPreWakeupIsToTurnOffFirst()) {
            return true;
        }
        return SessionPausedWindow::onInput(inputEvent);
    }

} // namespace gui
