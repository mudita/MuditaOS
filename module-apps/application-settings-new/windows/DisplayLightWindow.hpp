// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/GuiTimer.hpp>
#include <module-services/service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <module-apps/application-settings-new/widgets/SpinBox.hpp>
#include <module-apps/application-settings-new/ApplicationSettings.hpp>
#include <module-apps/application-settings-new/widgets/OptionSetting.hpp>
#include "BaseSettingsWindow.hpp"

namespace gui
{
    namespace lighting
    {
        constexpr inline auto AMBIENT_LIGHT_TIMER_MS = 2000;
    }

    class DisplayLightWindow : public BaseSettingsWindow
    {
      public:
        DisplayLightWindow(app::Application *app, app::settingsInterface::ScreenLightSettings *screenLightSettings);
        ~DisplayLightWindow();

      private:
        auto buildOptionsList() -> std::list<Option> override;
        void switchHandler(bool &onOffSwitch);

        void addBrightnessOption(std::list<Option> &);
        auto createBrightnessOption(int step) -> std::unique_ptr<SpinBoxOptionSettings>;
        bool isDisplayLightSwitchOn                                      = false;
        bool isAutoLightSwitchOn                                         = false;
        std::uint8_t brightnessValue                                     = 0;
        app::settingsInterface::ScreenLightSettings *screenLightSettings = nullptr;
        float ambientLight                                               = 0.0;
        sys::TimerHandle timerTask;
        [[nodiscard]] auto onTimerTimeout(Item &self, sys::Timer &task) -> bool;
    };
} // namespace gui
