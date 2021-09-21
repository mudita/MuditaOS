// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "backlight-handler/BacklightHandlerCommon.hpp"
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

namespace backlight
{
    namespace timers
    {
        constexpr auto screenLightTimerName    = "ScreenLightTimer";
        constexpr auto screenLightTimerTimeout = std::chrono::seconds(5);
    } // namespace timers

    HandlerCommon::HandlerCommon(std::shared_ptr<settings::Settings> settings, sys::Service *parent)
        : settings{std::move(settings)}, screenLightControl{std::make_shared<screen_light_control::ScreenLightControl>(
                                             parent)},
          screenLightTimer{std::make_shared<sys::TimerHandle>(sys::TimerFactory::createSingleShotTimer(
              parent, timers::screenLightTimerName, timers::screenLightTimerTimeout, [this](sys::Timer &t) {
                  if (getScreenAutoModeState() == screen_light_control::ScreenLightMode::Automatic &&
                      screenLightControl->isLightOn()) {
                      screenLightControl->processRequest(screen_light_control::Action::turnOff);
                  }
              }))}

    {}

    void HandlerCommon::init()
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
                screenLightTimer->start();
            }
            screenLightControl->processRequest(action);
        });
    }

    void HandlerCommon::processScreenRequest(screen_light_control::Action action,
                                             const screen_light_control::Parameters &params)
    {
        if (action == screen_light_control::Action::enableAutomaticMode) {
            startScreenLightTimer();
        }
        handleScreenLightSettings(action, params);
        screenLightControl->processRequest(action, params);
    }

    auto HandlerCommon::getValue(const std::string &path) const -> std::string
    {
        return settings->getValue(path);
    }

    void HandlerCommon::setValue(const std::string &path, const std::string &value)
    {
        settings->setValue(path, value);
    }

    void HandlerCommon::startScreenLightTimer()
    {
        screenLightTimer->start();
    }

    auto HandlerCommon::getScreenLightState() const noexcept -> bool
    {
        return utils::getNumericValue<bool>(getValue(settings::Brightness::state));
    }

    auto HandlerCommon::getScreenAutoModeState() const noexcept -> screen_light_control::ScreenLightMode
    {
        auto mode = utils::getNumericValue<int>(getValue(settings::Brightness::autoMode));
        return static_cast<screen_light_control::ScreenLightMode>(mode);
    }

    auto HandlerCommon::getScreenBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage
    {
        return utils::getNumericValue<float>(getValue(settings::Brightness::brightnessLevel));
    }

    void HandlerCommon::handleScreenLightRefresh()
    {
        if (getScreenLightState() && getScreenAutoModeState() == screen_light_control::ScreenLightMode::Automatic) {
            if (!screenLightControl->isLightOn()) {
                screenLightControl->processRequest(screen_light_control::Action::turnOn);
            }
            startScreenLightTimer();
        }
    }

    void HandlerCommon::handleScreenLightSettings(screen_light_control::Action action,
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
            screenLightTimer->start();
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
