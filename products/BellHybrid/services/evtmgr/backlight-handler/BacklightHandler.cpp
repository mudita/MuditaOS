// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <evtmgr/backlight-handler/BacklightHandler.hpp>

#include <screen-light-control/ScreenLightControl.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>

#include <Timers/TimerFactory.hpp>
#include <keymap/KeyMap.hpp>
#include <Utils.hpp>

namespace backlight
{
    namespace timers
    {
        constexpr auto screenLightTimerTimeout       = std::chrono::seconds{5};
        constexpr auto screenLightTimerHoldTimeout   = std::chrono::seconds{10};
    } // namespace timers

    Handler::Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent)
        : HandlerCommon(std::move(settings),
                        std::make_shared<bell::screen_light_control::ScreenLightController>(parent),
                        parent,
                        [this]([[maybe_unused]] sys::Timer &t) {
                            if (screenLightController->isLightOn()) {
                                backlightType = Type::Frontlight;
                                screenLightController->processRequest(screen_light_control::Action::turnOff);
                            }
                        })
    {}

    auto Handler::init() -> void
    {
        using namespace screen_light_control;

        settings->registerValueChange(settings::Brightness::brightnessLevel, [&](const std::string &value) {
            const ConstLinearProgressModeParameters params{utils::getNumericValue<float>(value)};
            screenLightController->processRequest(Action::setAutomaticModeParameters, Parameters(params));
        });

        settings->registerValueChange(settings::Brightness::autoMode, [&](const std::string &value) {
            onDemandModeOn = utils::getNumericValue<bool>(value);
        });
    }

    auto Handler::handleKeyPressed([[maybe_unused]] int key) -> void
    {
        if (!ignoreKeypress) {
            handleScreenLightRefresh(key);
        }
    }

    auto Handler::handleScreenLightRefresh([[maybe_unused]] int key) -> void
    {
        const auto controller = getScreenLightControl();
        const auto timer      = getScreenLightTimer();

        if (key != static_cast<int>(KeyMap::Frontlight) && backlightType != Type::BedsideLamp) {
            if (controller->isLightOn()) {
                timer->restart(timers::screenLightTimerHoldTimeout);
            }
            else if (!onDemandModeOn) {
                setKeyPressedModeFrontlightOn();
                controller->processRequest(screen_light_control::Action::turnOn);
                timer->restart(timers::screenLightTimerHoldTimeout);
            }
        }
    }

    auto Handler::handleScreenLight(Type type) -> void
    {
        const auto controller = getScreenLightControl();
        const auto timer      = getScreenLightTimer();

        auto lightTime = std::chrono::seconds(0);
        auto lightParams = screen_light_control::Parameters{};

        switch (type) {
        case Type::Frontlight:
            if (onDemandModeOn) {
                lightTime = timers::screenLightTimerTimeout;
            }
            else {
                lightTime = timers::screenLightTimerHoldTimeout;
            }
            break;
        case Type::BedsideLamp:
            lightTime   = getBedsideModeLightTime();
            lightParams = getBedsideModeLightParams();
            break;
        }

        if (controller->isLightOn() && type == Type::Frontlight) {
            setKeyPressedModeFrontlightOff();
            controller->processRequest(screen_light_control::Action::turnOff, lightParams);
            timer->stop();
        }
        else if (!controller->isLightOn()) {
            setKeyPressedModeFrontlightOn();
            controller->processRequest(screen_light_control::Action::turnOn, lightParams);
            timer->restart(lightTime);
        }
        backlightType = type;
    }

    auto Handler::processRequest(screen_light_control::Action action,
                                 const screen_light_control::Parameters &params,
                                 screen_light_control::Sender sender) -> void
    {
        switch (sender) {
        case screen_light_control::Sender::AlarmPrewakeup:
        case screen_light_control::Sender::Alarm:
        case screen_light_control::Sender::PowerNap:
            switch (action) {
            case screen_light_control::Action::turnOff:
                backlightType = Type::Frontlight; // Override bedside lamp for events with higher priority
                break;
            default:
                break;
            }
            processScreenRequest(action, params);
            break;
        default:
            processScreenRequest(action, params);
            break;
        }
    }

    auto Handler::processScreenRequest(screen_light_control::Action action,
                                       const screen_light_control::Parameters &params) -> void
    {
        handleScreenLightSettings(action, params);
        getScreenLightControl()->processRequest(action, params);
    }

    auto Handler::handleScreenLightSettings(screen_light_control::Action action,
                                            const screen_light_control::Parameters &params) -> void
    {
        switch (action) {
        case screen_light_control::Action::turnOn:
            if (params.hasManualModeParams()) {
                auto timer = getScreenLightTimer();
                timer->stop();
                backlightMode = BacklightMode::WithoutTimer;
            }
            break;
        case screen_light_control::Action::setAutomaticModeParameters:
            if (params.hasConstLinearProgressModeParams()) {
                backlightMode = BacklightMode::WithTimer;
                setValue(settings::Brightness::brightnessLevel,
                         utils::to_string(params.getConstLinearProgressModeParams().targetBrightness));
            }
            else if (params.hasLinearProgressModeParams()) {
                backlightMode = BacklightMode::WithoutTimer;
            }
            break;
        case screen_light_control::Action::enableAutomaticMode:
            onDemandModeOn = false;
            break;
        case screen_light_control::Action::disableAutomaticMode:
            onDemandModeOn = true;
            break;
        case screen_light_control::Action::ignoreKeypress:
            ignoreKeypress = true;
            break;
        case screen_light_control::Action::stopIgnoringKeypress:
            ignoreKeypress = false;
            break;
        default:
            break;
        }

        HandlerCommon::handleScreenLightSettings(action, params);
    }

    auto Handler::onScreenLightTurnedOn() -> void
    {
        if (backlightMode == BacklightMode::WithTimer && backlightType != Type::BedsideLamp) {
            startScreenLightTimer();
        }
    }

    auto Handler::setKeyPressedModeFrontlightOn() -> void
    {
        using namespace screen_light_control;

        const auto brightnessLevel = utils::getNumericValue<float>(getValue(settings::Brightness::brightnessLevel));
        const ConstLinearProgressModeParameters params{brightnessLevel};
        screenLightController->processRequest(Action::setAutomaticModeParameters, Parameters(params));
    }

    auto Handler::setKeyPressedModeFrontlightOff() -> void
    {
        using namespace screen_light_control;

        screenLightController->processRequest(Action::setAutomaticModeParameters,
                                              Parameters(screen_light_control::ConstLinearProgressModeParameters{0}));
    }

    auto Handler::getBedsideModeLightTime() const -> std::chrono::seconds
    {
        const auto bedsideTime = utils::toNumeric(getValue(settings::Brightness::bedsideTime));
        return std::chrono::minutes{bedsideTime};
    }

    auto Handler::getBedsideModeLightParams() const -> screen_light_control::Parameters
    {
        using namespace screen_light_control;

        const auto bedsideBrightness =
            utils::getNumericValue<float>(getValue(settings::Brightness::bedsideBrightnessLevel));
        const auto manualParams = ManualModeParameters{bedsideBrightness};
        return Parameters{manualParams};
    }
} // namespace backlight
