// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BacklightHandler.hpp"
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

namespace backlight
{
    namespace timers
    {
        constexpr auto keypadLightTimerName    = "KeypadLightTimer";
        constexpr auto screenLightTimerName    = "ScreenLightTimer";
        constexpr auto keypadLightTimerTimeout = std::chrono::seconds(5);
        constexpr auto screenLightTimerTimeout = std::chrono::seconds(5);
    } // namespace timers

    Handler::Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent)
        : settings{settings}, screenLightControl{std::make_unique<screen_light_control::ScreenLightControl>(settings,
                                                                                                            parent)},
          keypadLightTimer{
              sys::TimerFactory::createSingleShotTimer(parent,
                                                       timers::keypadLightTimerName,
                                                       timers::keypadLightTimerTimeout,
                                                       [this](sys::Timer &) { bsp::keypad_backlight::shutdown(); })},
          screenLightTimer{sys::TimerFactory::createSingleShotTimer(
              parent, timers::screenLightTimerName, timers::keypadLightTimerTimeout, [this](sys::Timer &) {
                  if (utils::getNumericValue<bool>(getValue(settings::Brightness::state)) &&
                      utils::getNumericValue<bool>(getValue(settings::Brightness::autoMode)) &&
                      screenLightControl->getLightState()) {
                      screenLightControl->processRequest(screen_light_control::Action::turnOff);
                  }
              })}

    {}

    void Handler::init()
    {
        screenLightControl->initFromSettings();
    }

    void Handler::processScreenRequest(screen_light_control::Action action)
    {
        if (action == screen_light_control::Action::enableAutomaticMode) {
            startScreenLightTimer();
        }
        handleScreenLightSettings(action);
        screenLightControl->processRequest(action);
    }

    void Handler::processScreenRequest(screen_light_control::ParameterizedAction action,
                                       screen_light_control::Parameters params)
    {
        handleScreenLightSettings(action, params);
        screenLightControl->processRequest(action, params);
    }

    void Handler::handleKeyPressed()
    {
        handleKeypadLightRefresh();
        handleScreenLightRefresh();
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

    auto Handler::getValue(std::string path) const -> std::string
    {
        return settings->getValue(path);
    }

    void Handler::setValue(std::string path, std::string value)
    {
        settings->setValue(path, value);
    }

    void Handler::startKeypadLightTimer()
    {
        keypadLightTimer.start();
    }

    void Handler::startScreenLightTimer()
    {
        screenLightTimer.start();
    }

    auto Handler::getScreenLightState() const noexcept -> bool
    {
        return screenLightControl->getLightState();
    }

    auto Handler::getScreenAutoModeState() const noexcept -> screen_light_control::ScreenLightMode
    {
        return screenLightControl->getAutoModeState();
    }

    auto Handler::getScreenBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage
    {
        return screenLightControl->getBrightnessValue();
    }

    void Handler::handleKeypadLightRefresh()
    {
        if (keypadLightState == bsp::keypad_backlight::State::activeMode && !isKeypadLightInCallMode) {
            bsp::keypad_backlight::turnOnAll();
            startKeypadLightTimer();
        }
    }

    void Handler::handleScreenLightRefresh()
    {
        if (utils::getNumericValue<bool>(getValue(settings::Brightness::state)) &&
            utils::getNumericValue<bool>(getValue(settings::Brightness::autoMode))) {

            if (!screenLightControl->getLightState()) {
                screenLightControl->processRequest(screen_light_control::Action::turnOn);
            }
            startScreenLightTimer();
        }
    }

    void Handler::handleScreenLightSettings(screen_light_control::Action action)
    {
        switch (action) {
        case screen_light_control::Action::enableAutomaticMode:
            setValue(settings::Brightness::autoMode, utils::to_string(true));
            break;
        case screen_light_control::Action::disableAutomaticMode:
            setValue(settings::Brightness::autoMode, utils::to_string(false));
            screenLightControl->processRequest(screen_light_control::ParameterizedAction::setManualModeBrightness,
                                               screen_light_control::Parameters{utils::getNumericValue<float>(
                                                   getValue(settings::Brightness::brightnessLevel))});
            break;
        case screen_light_control::Action::turnOff:
            setValue(settings::Brightness::state, utils::to_string(false));
            break;
        case screen_light_control::Action::turnOn:
            setValue(settings::Brightness::state, utils::to_string(true));
            break;
        }
    }

    void Handler::handleScreenLightSettings(screen_light_control::ParameterizedAction action,
                                            screen_light_control::Parameters params)
    {
        setValue(settings::Brightness::brightnessLevel, utils::to_string(params.manualModeBrightness));
    }
} // namespace backlight
