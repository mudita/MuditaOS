// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingSettingsWindow.hpp"
#include <BellOnBoardingNames.hpp>

namespace gui
{
    OnBoardingSettingsWindow::OnBoardingSettingsWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_settings::TimeUnitsWindowContract::Presenter> &&windowPresenter,
        std::string name)
        : BellSettingsTimeUnitsWindow(app, std::move(windowPresenter), name)
    {
        finishedCallback = [this]() { application->switchWindow(window::name::finalizeOnBoardingWindow); };
        returnCallback   = [this]() { application->switchWindow(window::name::onBoardingShortcutsOptionWindow); };
    }
} // namespace gui
