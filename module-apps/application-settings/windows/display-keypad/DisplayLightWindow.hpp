// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/ApplicationSettings.hpp>
#include <application-settings/widgets/SpinBoxOptionSettings.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

#include <GuiTimer.hpp>

namespace gui
{
    namespace lighting
    {
        constexpr inline auto AMBIENT_LIGHT_TIMER_MS = 2000;
        constexpr inline auto LIGHT_CONTROL_STEP     = 20;
    } // namespace lighting

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
        bool isDisplayLightSwitchOn                                = false;
        bool isAutoLightSwitchOn                                   = false;
        bsp::eink_frontlight::BrightnessPercentage brightnessValue = 0.0;

        app::settingsInterface::ScreenLightSettings *screenLightSettings = nullptr;
        float ambientLight                                               = 0.0;
        sys::TimerHandle timerTask;
        [[nodiscard]] auto onTimerTimeout(Item &self, sys::Timer &task) -> bool;
    };
} // namespace gui
