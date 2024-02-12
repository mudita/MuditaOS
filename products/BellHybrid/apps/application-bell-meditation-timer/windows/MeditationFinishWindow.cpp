// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationFinishWindow.hpp"
#include <keymap/KeyMap.hpp>
#include <products/BellHybrid/apps/application-bell-meditation-timer/data/MeditationCommon.hpp>

namespace gui
{
    MeditationFinishWindow::MeditationFinishWindow(
        app::ApplicationCommon *app,
        const std::string &name,
        std::shared_ptr<app::meditation::MeditationBasicsContract::Presenter> presenter)
        : BellFinishedWindow(app, name), presenter{std::move(presenter)}
    {}
    bool MeditationFinishWindow::onInput(const InputEvent &inputEvent)
    {
        const auto key = mapKey(inputEvent.getKeyCode());
        if (inputEvent.isShortRelease() && key != KeyMap::Frontlight &&
            presenter->handleIfPreWakeupIsToTurnOffFirst()) {
            return true;
        }
        return BellFinishedWindow::onInput(inputEvent);
    }

} // namespace gui
