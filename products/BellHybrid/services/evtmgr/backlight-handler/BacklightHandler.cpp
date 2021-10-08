// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <evtmgr/backlight-handler/BacklightHandler.hpp>

#include <screen-light-control/ScreenLightControl.hpp>

#include <service-db/Settings.hpp>

#include <Timers/TimerFactory.hpp>
#include <keymap/KeyMap.hpp>

namespace backlight
{
    namespace timers
    {
        constexpr auto screenLightTimerTimeout     = std::chrono::seconds(5);
        constexpr auto screenLightTimerHoldTimeout = std::chrono::seconds(10);
    } // namespace timers

    Handler::Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent)
        : HandlerCommon(
              std::move(settings), std::make_shared<bell::screen_light_control::ScreenLightController>(parent), parent)
    {}

    void Handler::handleKeyPressed([[maybe_unused]] int key)
    {
        handleScreenLightRefresh(key);
    }

    void Handler::handleScreenLightRefresh([[maybe_unused]] int key)
    {
        auto controller = getScreenLightControl();
        auto timer      = getScreenLightTimer();
        if (!controller->isLightOn() && (key == static_cast<int>(KeyMap::Frontlight))) {
            controller->processRequest(screen_light_control::Action::turnOn);
            timer->restart(timers::screenLightTimerTimeout);
        }
        else if (controller->isLightOn()) {
            if (controller->isAutoModeOn() || key == static_cast<int>(KeyMap::Frontlight)) {
                controller->processRequest(screen_light_control::Action::turnOff);
                timer->stop();
            }
            else {
                timer->restart(timers::screenLightTimerHoldTimeout);
            }
        }
    }

    void Handler::processScreenRequest(screen_light_control::Action action,
                                       const screen_light_control::Parameters &params)
    {
        handleScreenLightSettings(action, params);
        getScreenLightControl()->processRequest(action, params);
    }

    void Handler::onScreenLightTurnedOn()
    {
        if (!getScreenLightControl()->isAutoModeOn()) {
            startScreenLightTimer();
        }
    }
} // namespace backlight
