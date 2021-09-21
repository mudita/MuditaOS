// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <Timers/TimerHandle.hpp>
#include <Service/ServiceProxy.hpp>
#include <backlight-handler/BacklightHandlerCommon.hpp>

namespace settings
{
    class Settings;
} // namespace settings

namespace backlight
{
    /// @brief Backlight events handler
    class HandlerBell : public Handler
    {
      public:
        HandlerBell(std::shared_ptr<settings::Settings> settings, sys::Service *parent);

        void handleKeyPressed(const int key = 0);

      private:
        std::shared_ptr<settings::Settings> settings;
        std::unique_ptr<screen_light_control::ScreenLightControl> screenLightControl;
        /// Timer that keeps screen backlight on for a certain time if there was key pressed
        sys::TimerHandle screenLightTimer;
        void startScreenLightTimer();
        void handleScreenLightRefresh(const int key = 0);
        void handleScreenLightSettings(screen_light_control::Action action,
                                       const screen_light_control::Parameters &params);
    };
} // namespace backlight
