// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <evtmgr/BacklightHandler.hpp>
#include <screen-light-control/ScreenLightControl.hpp>

#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

namespace backlight
{
    namespace timers
    {
        constexpr auto keypadLightTimerName    = "KeypadLightTimer";
        constexpr auto lightTimerTimeout        = std::chrono::seconds(20);
        constexpr auto lightFadeoutTimerTimeout = std::chrono::seconds(10);
    } // namespace timers

    namespace
    {
        constexpr std::array exclusions = {bsp::KeyCodes::Torch};

        [[nodiscard]] bool isKeyOnExclusionList(bsp::KeyCodes key)
        {
            for (const auto &exclusion : exclusions) {
                if (key == exclusion) {
                    return true;
                }
            }
            return false;
        }
    } // namespace

    Handler::Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent)
        : HandlerCommon(std::move(settings),
                        std::make_shared<pure::screen_light_control::ScreenLightController>(parent),
                        parent,
                        [this](sys::Timer &t) { handleScreenLightTimeout(); }),
          keypadLightTimer{sys::TimerFactory::createSingleShotTimer(
              parent, timers::keypadLightTimerName, timers::lightTimerTimeout, [this](sys::Timer &) {
                  bsp::keypad_backlight::shutdown();
              })}
    {}

    void Handler::init()
    {
        using namespace screen_light_control;

        settings->registerValueChange(settings::Brightness::brightnessLevel, [&](const std::string &value) {
            ManualModeParameters params{utils::getNumericValue<float>(value)};
            screenLightController->processRequest(Action::setManualModeBrightness, Parameters(params));
        });

        settings->registerValueChange(settings::Brightness::autoMode, [&]([[maybe_unused]] const std::string &value) {
            const auto action = getScreenAutoModeState() == ScreenLightMode::Automatic ? Action::enableAutomaticMode
                                                                                       : Action::disableAutomaticMode;
            screenLightController->processRequest(action);
        });

        settings->registerValueChange(settings::Brightness::state, [&]([[maybe_unused]] const std::string &value) {
            const auto action = getScreenLightState() ? Action::turnOn : Action::turnOff;
            if (action == Action::turnOn) {
                onScreenLightTurnedOn();
            }
            screenLightController->processRequest(action);
        });
    }

    void Handler::handleKeyPressed(bsp::KeyCodes key)
    {
        if (!isKeyOnExclusionList(key)) {
            handleKeypadLightRefresh();
            handleScreenLightRefresh();
        }
    }

    void Handler::processScreenRequest(screen_light_control::Action action,
                                       const screen_light_control::Parameters &params)
    {
        if (action == screen_light_control::Action::enableAutomaticMode) {
            getScreenLightTimer()->restart(timers::lightTimerTimeout);
        }
        handleScreenLightSettings(action, params);
        getScreenLightControl()->processRequest(action, params);
    }

    void Handler::stopKeypadTimer()
    {
        if (keypadLightState == bsp::keypad_backlight::State::activeMode) {
            keypadLightTimer.stop();
        }
    }

    void Handler::setKeypadLightInCallMode(bool value) noexcept
    {
        isKeypadLightInCallMode = value;
    }

    void Handler::setKeypadLightState(bsp::keypad_backlight::State value) noexcept
    {
        keypadLightState = value;
    }

    auto Handler::processKeypadRequest(bsp::keypad_backlight::Action action) -> bool
    {
        switch (action) {
        case bsp::keypad_backlight::Action::turnOn: {
            stopKeypadTimer();
            setKeypadLightState(bsp::keypad_backlight::State::on);
            return bsp::keypad_backlight::turnOnAll();
        }
        case bsp::keypad_backlight::Action::turnOff: {
            stopKeypadTimer();
            setKeypadLightState(bsp::keypad_backlight::State::off);
            return bsp::keypad_backlight::shutdown();
        }
        case bsp::keypad_backlight::Action::checkState: {
            return bsp::keypad_backlight::checkState();
        }
        case bsp::keypad_backlight::Action::turnOnActiveMode: {
            setKeypadLightState(bsp::keypad_backlight::State::activeMode);
            const auto response = bsp::keypad_backlight::turnOnAll();
            startKeypadLightTimer();
            return response;
        }
        case bsp::keypad_backlight::Action::turnOnCallMode: {
            if (keypadLightTimer.isActive()) {
                keypadLightTimer.stop();
            }
            return bsp::keypad_backlight::turnOnFunctionKeysBacklight();
        }
        case bsp::keypad_backlight::Action::turnOffCallMode: {
            setKeypadLightInCallMode(false);
            restoreKeypadLightState();
            return true;
        }
        }
        return false;
    }

    void Handler::restoreKeypadLightState()
    {
        switch (keypadLightState) {
        case bsp::keypad_backlight::State::off:
            processKeypadRequest(bsp::keypad_backlight::Action::turnOff);
            break;
        case bsp::keypad_backlight::State::on:
            processKeypadRequest(bsp::keypad_backlight::Action::turnOn);
            break;
        case bsp::keypad_backlight::State::activeMode:
            processKeypadRequest(bsp::keypad_backlight::Action::turnOnActiveMode);
            break;
        }
    }

    void Handler::startKeypadLightTimer()
    {
        keypadLightTimer.start();
    }

    void Handler::handleKeypadLightRefresh()
    {
        if (keypadLightState == bsp::keypad_backlight::State::activeMode && !isKeypadLightInCallMode) {
            bsp::keypad_backlight::turnOnAll();
            startKeypadLightTimer();
        }
    }

    void Handler::handleScreenLightRefresh([[maybe_unused]] const int key)
    {
        if (getScreenLightState()) {
            if (!getScreenLightControl()->isLightOn() || getScreenLightControl()->isFadeOutOngoing()) {
                getScreenLightControl()->processRequest(screen_light_control::Action::turnOn);
            }
            getScreenLightTimer()->restart(timers::lightTimerTimeout);
        }
    }

    void Handler::handleScreenLightTimeout()
    {
        if (getScreenAutoModeState() == screen_light_control::ScreenLightMode::Automatic) {
            if (screenLightController->isLightOn()) {
                if (screenLightController->isFadeOutOngoing()) {
                    screenLightController->processRequest(screen_light_control::Action::turnOff);
                }
                else {
                    screenLightController->processRequest(screen_light_control::Action::fadeOut);
                    getScreenLightTimer()->restart(timers::lightFadeoutTimerTimeout);
                }
            }
        }
        else if (getScreenAutoModeState() == screen_light_control::ScreenLightMode::Manual) {
            if (screenLightController->isLightOn()) {
                screenLightController->processRequest(screen_light_control::Action::turnOff);
            }
        }
    }

    void Handler::onScreenLightTurnedOn()
    {
        getScreenLightTimer()->restart(timers::lightTimerTimeout);
    }

} // namespace backlight
