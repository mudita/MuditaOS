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

        void handleKeyPressed(int key = 0);

        void processScreenRequest(screen_light_control::Action action,
                                  const screen_light_control::Parameters &params) override;

      private:
        void handleScreenLightRefresh(int key = 0);

        void onScreenLightTurnedOn() override;
    };
} // namespace backlight
