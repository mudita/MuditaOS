// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <Timers/TimerHandle.hpp>

namespace settings
{
    class Settings;
} // namespace settings

namespace backlight
{
    class SettingsInterface
    {
      public:
        virtual ~SettingsInterface() = default;

      protected:
        virtual auto getValue(std::string path) const -> std::string = 0;
        virtual void setValue(std::string path, std::string value)   = 0;
    };

    /// @brief Backlight events handler
    class Handler : public SettingsInterface
    {
      public:
        Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent);

        /// Process request of the screen light control
        /// @screen_light_control::Action an action to perform
        void processScreenRequest(screen_light_control::Action action);

        /// Process request of the screen light control with specified parameters
        /// @screen_light_control::ParameterizedAction an action to perform
        /// @screen_light_control::Parameters parameters being set
        void processScreenRequest(screen_light_control::ParameterizedAction action,
                                  screen_light_control::Parameters params);

        void handleKeyPressed();
        /// Process request of the keypad light control
        /// @screen_light_control::ParameterizedAction an action to perform
        /// @return True if request was processed succesfully, false otherwise
        auto processKeypadRequest(bsp::keypad_backlight::Action action) -> bool;

        [[nodiscard]] auto getScreenLightState() const noexcept -> bool;
        [[nodiscard]] auto getScreenAutoModeState() const noexcept -> screen_light_control::ScreenLightMode;
        [[nodiscard]] auto getScreenBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage;

      protected:
        [[nodiscard]] auto getValue(std::string path) const -> std::string override;
        void setValue(std::string path, std::string value) override;

      private:
        std::shared_ptr<settings::Settings> settings;
        std::unique_ptr<screen_light_control::ScreenLightControl> screenLightControl;
        /// Timer that keeps key backlight on for a certain time if there was key pressed
        sys::TimerHandle keypadLightTimer;
        /// Timer that keeps screen backlight on for a certain time if there was key pressed
        sys::TimerHandle screenLightTimer;
        bsp::keypad_backlight::State keypadLightState = bsp::keypad_backlight::State::off;
        bool isKeypadLightInCallMode                  = false;

        void startKeypadLightTimer();
        void startScreenLightTimer();
        void stopKeypadTimer();
        void setKeypadLightInCallMode(bool value) noexcept;
        void setKeypadLightState(bsp::keypad_backlight::State value) noexcept;
        void restoreKeypadLightState();
        void handleKeypadLightRefresh();
        void handleScreenLightRefresh();
        void handleScreenLightSettings(screen_light_control::Action action);
        void handleScreenLightSettings(screen_light_control::ParameterizedAction action,
                                       screen_light_control::Parameters params);
    };
} // namespace backlight
