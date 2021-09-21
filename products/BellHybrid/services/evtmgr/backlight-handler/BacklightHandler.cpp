// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <evtmgr/BacklightHandler.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>
#include <keymap/KeyMap.hpp>

namespace backlight
{
    namespace timers
    {
        constexpr auto screenLightTimerTimeout     = std::chrono::seconds(5);
        constexpr auto screenLightTimerHoldTimeout = std::chrono::seconds(10);
    } // namespace timers

    Handler::Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent)
        : HandlerCommon(settings, parent), screenLightControl{getScreenLightControl()}, screenLightTimer{
                                                                                            getScreenLightTimer()}
    {}

    void Handler::handleKeyPressed([[maybe_unused]] const int key)
    {
        handleScreenLightRefresh(key);
    }

    void Handler::handleScreenLightRefresh([[maybe_unused]] const int key)
    {
        if (getScreenAutoModeState() == screen_light_control::ScreenLightMode::Automatic) {
            if (!screenLightControl->isLightOn() && (key == static_cast<int>(KeyMap::Frontlight))) {
                screenLightControl->processRequest(screen_light_control::Action::turnOn);
                screenLightTimer->restart(timers::screenLightTimerTimeout);
            }
            else if (screenLightControl->isLightOn() && (key != static_cast<int>(KeyMap::Frontlight))) {
                screenLightTimer->restart(timers::screenLightTimerHoldTimeout);
            }
            else if (key == static_cast<int>(KeyMap::Frontlight)) {
                screenLightControl->processRequest(screen_light_control::Action::turnOff);
                screenLightTimer->stop();
            }
        }
    }
} // namespace backlight
