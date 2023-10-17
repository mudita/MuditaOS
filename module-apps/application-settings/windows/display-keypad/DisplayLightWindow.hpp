// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
        DisplayLightWindow(app::ApplicationCommon *app,
                           app::settingsInterface::ScreenLightSettings *screenLightSettings);
#if DEVELOPER_SETTINGS_OPTIONS == 1
        ~DisplayLightWindow();
#endif // DEVELOPER_SETTINGS_OPTIONS

      private:
        auto buildOptionsList() -> std::list<Option> override;
        void switchHandler(bool &onOffSwitch);

        void addBrightnessOption(std::list<Option> &);
        auto createBrightnessOption(int step) -> std::unique_ptr<SpinBoxOptionSettings>;

#if DEVELOPER_SETTINGS_OPTIONS == 1
        [[nodiscard]] auto onTimerTimeout(Item &self, sys::Timer &task) -> bool;
#endif // DEVELOPER_SETTINGS_OPTIONS

        bool isDisplayLightSwitchOn                                      = false;
        bool isAutoLightSwitchOn                                         = false;
        bsp::eink_frontlight::BrightnessPercentage brightnessValue       = 0.0;
        app::settingsInterface::ScreenLightSettings *screenLightSettings = nullptr;
        float ambientLight                                               = 0.0;
#if DEVELOPER_SETTINGS_OPTIONS == 1
        sys::TimerHandle timerTask;
#endif // DEVELOPER_SETTINGS_OPTIONS
        OptionWindowDestroyer rai_destroyer = OptionWindowDestroyer(*this);
    };
} // namespace gui
