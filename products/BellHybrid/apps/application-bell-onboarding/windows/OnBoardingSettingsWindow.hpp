// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-bell-settings/windows/advanced/BellSettingsTimeUnitsWindow.hpp>

namespace gui
{
    class OnBoardingSettingsWindow : public BellSettingsTimeUnitsWindow
    {
      public:
        OnBoardingSettingsWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::bell_settings::TimeUnitsWindowContract::Presenter> &&windowPresenter,
            std::string name = gui::window::name::bellSettingsTimeUnits);
    };

} // namespace gui
