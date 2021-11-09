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
    class Handler : public HandlerCommon
    {
      public:
        Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent);

        void init() override;
        void handleKeyPressed(int key = 0);
        /// Process request of the keypad light control
        /// @keypad_backlight::action an action to perform
        /// @return True if request was processed successfully, false otherwise
        auto processKeypadRequest(bsp::keypad_backlight::Action action) -> bool;

        void processScreenRequest(screen_light_control::Action action,
                                  const screen_light_control::Parameters &params) override;

      private:
        /// Timer that keeps key backlight on for a certain time if there was key pressed
        sys::TimerHandle keypadLightTimer;
        /// Timer that keeps screen backlight on for a certain time if there was key pressed
        sys::TimerHandle screenLightTimer;
        bsp::keypad_backlight::State keypadLightState = bsp::keypad_backlight::State::off;
        bool isKeypadLightInCallMode                  = false;

        void startKeypadLightTimer();
        void stopKeypadTimer();
        void setKeypadLightInCallMode(bool value) noexcept;
        void setKeypadLightState(bsp::keypad_backlight::State value) noexcept;
        void restoreKeypadLightState();
        void handleKeypadLightRefresh();
        void handleScreenLightRefresh(int key = 0);
        void onScreenLightTurnedOn() override;
    };
} // namespace backlight
