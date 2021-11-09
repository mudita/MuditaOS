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
        HandlerCommon(std::shared_ptr<settings::Settings> settings,
                      std::shared_ptr<screen_light_control::ScreenLightController> screenLightController,
                      sys::Service *parent,
                      sys::timer::TimerCallback &&screenLightTimerCallback);

        /// initialise in InitHandler when Service is ready
        virtual void init() = 0;

        /// Process request of the screen light control
        /// @screen_light_control::Action an action to perform
        /// @screen_light_control::Parameters parameters being set
        virtual void processScreenRequest(screen_light_control::Action action,
                                          const screen_light_control::Parameters &params) = 0;

        [[nodiscard]] auto getScreenLightState() const noexcept -> bool;
        [[nodiscard]] auto getScreenAutoModeState() const noexcept -> screen_light_control::ScreenLightMode;
        [[nodiscard]] auto getScreenBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage;

      private:
        virtual void onScreenLightTurnedOn() = 0;

      protected:
        std::shared_ptr<settings::Settings> settings;
        std::shared_ptr<screen_light_control::ScreenLightController> screenLightController;

      public:
        auto getScreenLightTimer() noexcept -> std::shared_ptr<sys::TimerHandle>
        {
            return screenLightTimer;
        }
        auto getScreenLightControl() noexcept -> std::shared_ptr<screen_light_control::ScreenLightController>
        {
            return screenLightController;
        }
        virtual void handleScreenLightSettings(screen_light_control::Action action,
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
