// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BacklightHandler.hpp"
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

namespace backlight
{
    namespace timers
    {
        constexpr auto keypadLightTimerName    = "KeypadLightTimer";
        constexpr auto screenLightTimerName    = "ScreenLightTimer";
        constexpr auto keypadLightTimerTimeout = std::chrono::seconds(5);
        // constexpr auto screenLightTimerTimeout = std::chrono::seconds(5);
    } // namespace timers

    Handler::Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent)
        : settings{std::move(settings)}, screenLightControl{std::make_unique<screen_light_control::ScreenLightControl>(
                                             parent)},
          keypadLightTimer{sys::TimerFactory::createSingleShotTimer(
              parent,
              timers::keypadLightTimerName,
              timers::keypadLightTimerTimeout,
              [&](sys::Timer & /*unused*/) { bsp::keypad_backlight::shutdown(); })},
          screenLightTimer{sys::TimerFactory::createSingleShotTimer(
              parent, timers::screenLightTimerName, timers::keypadLightTimerTimeout, [this](sys::Timer &t) {
                  if (getScreenLightState() &&
                      getScreenAutoModeState() == screen_light_control::ScreenLightMode::Automatic &&
                      screenLightControl->isLightOn()) {
                      screenLightControl->processRequest(screen_light_control::Action::turnOff);
                  }
              })}

    {}

    void Handler::init()
    {
        using namespace screen_light_control;
        settings->registerValueChange(settings::Brightness::brightnessLevel, [&](const std::string &value) {
            ManualModeParameters params{utils::getNumericValue<float>(value)};
            screenLightControl->processRequest(Action::setManualModeBrightness, Parameters(params));
        });

        settings->registerValueChange(settings::Brightness::autoMode, [&]([[maybe_unused]] const std::string &value) {
            const auto action = getScreenAutoModeState() == ScreenLightMode::Automatic ? Action::enableAutomaticMode
                                                                                       : Action::disableAutomaticMode;
            screenLightControl->processRequest(action);
        });

        settings->registerValueChange(settings::Brightness::state, [&]([[maybe_unused]] const std::string &value) {
            const auto action = getScreenLightState() ? Action::turnOn : Action::turnOff;
            if (action == Action::turnOn) {
                screenLightTimer.start();
            }
            screenLightControl->processRequest(action);
        });
    }

    void Handler::processScreenRequest(screen_light_control::Action action,
                                       const screen_light_control::Parameters &params)
    {
        if (action == screen_light_control::Action::enableAutomaticMode) {
            startScreenLightTimer();
        }
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

    auto Handler::getValue(const std::string &path) const -> std::string
    {
        return settings->getValue(path);
    }

    void Handler::setValue(const std::string &path, const std::string &value)
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
        return utils::getNumericValue<bool>(getValue(settings::Brightness::state));
    }

    auto Handler::getScreenAutoModeState() const noexcept -> screen_light_control::ScreenLightMode
    {
        auto mode = utils::getNumericValue<int>(getValue(settings::Brightness::autoMode));
        return static_cast<screen_light_control::ScreenLightMode>(mode);
    }

    auto Handler::getScreenBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage
    {
        return utils::getNumericValue<float>(getValue(settings::Brightness::brightnessLevel));
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
        if (getScreenLightState() && getScreenAutoModeState() == screen_light_control::ScreenLightMode::Automatic) {
            if (!screenLightControl->isLightOn()) {
                screenLightControl->processRequest(screen_light_control::Action::turnOn);
            }
            startScreenLightTimer();
        }
    }

    void Handler::handleScreenLightSettings(screen_light_control::Action action,
                                            const screen_light_control::Parameters &params)
    {
        switch (action) {
        case screen_light_control::Action::enableAutomaticMode:
            setValue(settings::Brightness::autoMode,
                     utils::to_string(static_cast<int>(screen_light_control::ScreenLightMode::Automatic)));
            break;
        case screen_light_control::Action::disableAutomaticMode:
            setValue(settings::Brightness::autoMode,
                     utils::to_string(static_cast<int>(screen_light_control::ScreenLightMode::Manual)));
            break;
        case screen_light_control::Action::turnOff:
            setValue(settings::Brightness::state, utils::to_string(false));
            break;
        case screen_light_control::Action::turnOn:
            setValue(settings::Brightness::state, utils::to_string(true));
            screenLightTimer.start();
            break;
        case screen_light_control::Action::setManualModeBrightness:
            if (params.hasManualModeParams()) {
                setValue(settings::Brightness::brightnessLevel,
                         utils::to_string(params.getManualModeParams().manualModeBrightness));
            }
            else {
                LOG_ERROR("Missing ManualModeBrightness value, change request ignored");
            }
            break;
        default:
            break;
        }
    }
} // namespace backlight
