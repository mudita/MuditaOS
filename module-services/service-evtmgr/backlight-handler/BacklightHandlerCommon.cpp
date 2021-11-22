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

    HandlerCommon::HandlerCommon(std::shared_ptr<settings::Settings> settings,
                                 std::shared_ptr<screen_light_control::ScreenLightController> screenLightController,
                                 sys::Service *parent,
                                 sys::timer::TimerCallback &&screenLightTimerCallback)
        : settings{std::move(settings)}, screenLightController{std::move(screenLightController)},
          screenLightTimer{std::make_shared<sys::TimerHandle>(
              sys::TimerFactory::createSingleShotTimer(parent,
                                                       timers::screenLightTimerName,
                                                       timers::screenLightTimerTimeout,
                                                       std::move(screenLightTimerCallback)))}
    {}

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
            if (!screenLightController->isLightOn()) {
                screenLightController->processRequest(screen_light_control::Action::turnOn);
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
            onScreenLightTurnedOn();
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
