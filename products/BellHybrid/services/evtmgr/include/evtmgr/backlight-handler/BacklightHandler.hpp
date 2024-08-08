// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
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
    enum class Type
    {
        Frontlight,
        BedsideLamp
    };

    enum class BacklightMode
    {
        WithTimer,
        WithoutTimer
    };

    /// @brief Backlight events handler
    class Handler : public HandlerCommon
    {
      public:
        Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent);

        auto handleKeyPressed(int key = 0) -> void;

        auto handleScreenLight(Type type) -> void;

        auto processRequest(screen_light_control::Action action,
                            const screen_light_control::Parameters &params,
                            screen_light_control::Sender sender) -> void;

        auto init() -> void override;

        auto processScreenRequest(screen_light_control::Action action, const screen_light_control::Parameters &params)
            -> void override;

        auto handleScreenLightSettings(screen_light_control::Action action,
                                       const screen_light_control::Parameters &params) -> void override;

      private:
        auto handleScreenLightRefresh(int key = 0) -> void;

        auto onScreenLightTurnedOn() -> void override;

        auto setKeyPressedModeFrontlightOn() -> void;
        auto setKeyPressedModeFrontlightOff() -> void;

        [[nodiscard]] auto getBedsideModeLightTime() const -> std::chrono::seconds;
        [[nodiscard]] auto getBedsideModeLightParams() const -> screen_light_control::Parameters;

        bool ignoreKeypress         = false;
        bool onDemandModeOn         = true;
        BacklightMode backlightMode = BacklightMode::WithTimer;
        Type backlightType          = Type::Frontlight;
    };
} // namespace backlight
