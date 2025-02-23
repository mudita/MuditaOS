// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "OnBoardingSettingsWindow.hpp"
#include <BellOnBoardingNames.hpp>

namespace gui
{
    OnBoardingSettingsWindow::OnBoardingSettingsWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_settings::TimeUnitsWindowContract::Presenter> &&windowPresenter,
        std::string name)
        : BellSettingsTimeUnitsWindow(app, std::move(windowPresenter), std::move(name))
    {
        finishedCallback = [this]() { application->switchWindow(window::name::onBoardingUpdateInstructionWindow); };
        returnCallback   = [this]() { application->switchWindow(window::name::onBoardingShortcutsOptionWindow); };
    }
} // namespace gui
