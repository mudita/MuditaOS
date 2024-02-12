// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationSessionPausedWindow.hpp"
#include <products/BellHybrid/apps/application-bell-meditation-timer/data/MeditationCommon.hpp>
#include <keymap/KeyMap.hpp>

namespace gui
{
    MeditationSessionPausedWindow::MeditationSessionPausedWindow(
        app::ApplicationCommon *app, std::shared_ptr<app::meditation::MeditationBasicsContract::Presenter> presenter)
        : SessionPausedWindow(app, app::meditation::windows::meditationSessionPaused), presenter{std::move(presenter)}
    {}

    bool MeditationSessionPausedWindow::onInput(const InputEvent &inputEvent)
    {
        const auto key = mapKey(inputEvent.getKeyCode());
        if (inputEvent.isShortRelease() && key != KeyMap::Frontlight &&
            presenter->handleIfPreWakeupIsToTurnOffFirst()) {
            return true;
        }
        return SessionPausedWindow::onInput(inputEvent);
    }

} // namespace gui
