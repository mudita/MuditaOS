// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <Timers/TimerHandle.hpp>
#include <Service/ServiceProxy.hpp>

namespace settings
{
    class Settings;
} // namespace settings

namespace backlight
{
    /// @brief Backlight events handler
    class Handler
    {
      public:
        Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent);

        /// initialise in InitHandler when Service is ready
        void init();

        /// Process request of the screen light control
        /// @screen_light_control::Action an action to perform
        /// @screen_light_control::Parameters parameters being set
        void processScreenRequest(screen_light_control::Action action, const screen_light_control::Parameters &params);

        void handleKeyPressed(const int key = 0);
        /// Process request of the keypad light control
        /// @keypad_backlight::action an action to perform
        /// @return True if request was processed successfully, false otherwise
        auto processKeypadRequest(bsp::keypad_backlight::Action action) -> bool;

        [[nodiscard]] auto getScreenLightState() const noexcept -> bool;
        [[nodiscard]] auto getScreenAutoModeState() const noexcept -> screen_light_control::ScreenLightMode;
        [[nodiscard]] auto getScreenBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage;

      protected:
        [[nodiscard]] auto getValue(const std::string &path) const -> std::string;
        void setValue(const std::string &path, const std::string &value);

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
        void handleScreenLightRefresh(const int key = 0);
        void handleScreenLightSettings(screen_light_control::Action action,
                                       const screen_light_control::Parameters &params);
    };
} // namespace backlight
