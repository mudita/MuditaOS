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
    class HandlerCommon
    {
      public:
        HandlerCommon(std::shared_ptr<settings::Settings> settings, sys::Service *parent);

        /// initialise in InitHandler when Service is ready
        void init();

        /// Process request of the screen light control
        /// @screen_light_control::Action an action to perform
        /// @screen_light_control::Parameters parameters being set
        void processScreenRequest(screen_light_control::Action action, const screen_light_control::Parameters &params);

        [[nodiscard]] auto getScreenLightState() const noexcept -> bool;
        [[nodiscard]] auto getScreenAutoModeState() const noexcept -> screen_light_control::ScreenLightMode;
        [[nodiscard]] auto getScreenBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage;

      private:
        std::shared_ptr<settings::Settings> settings;
        std::shared_ptr<screen_light_control::ScreenLightControl> screenLightControl;

      public:
        auto getScreenLightTimer() noexcept -> std::shared_ptr<sys::TimerHandle>
        {
            return screenLightTimer;
        }
        auto getScreenLightControl() noexcept -> std::shared_ptr<screen_light_control::ScreenLightControl>
        {
            return screenLightControl;
        }
        void handleScreenLightSettings(screen_light_control::Action action,
                                       const screen_light_control::Parameters &params);
        void handleScreenLightRefresh();

      protected:
        [[nodiscard]] auto getValue(const std::string &path) const -> std::string;
        void setValue(const std::string &path, const std::string &value);

        /// Timer that keeps screen backlight on for a certain time if there was key pressed
        std::shared_ptr<sys::TimerHandle> screenLightTimer;

        void startScreenLightTimer();
    };
} // namespace backlight
