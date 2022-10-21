// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
        constexpr auto screenLightTimerTimeout       = std::chrono::seconds(5);
        constexpr auto screenLightTimerHoldTimeout   = std::chrono::seconds(10);
        constexpr auto screenLightBedsideLampTimeout = std::chrono::minutes(10);
    } // namespace timers

    Handler::Handler(std::shared_ptr<settings::Settings> settings, sys::Service *parent)
        : HandlerCommon(std::move(settings),
                        std::make_shared<bell::screen_light_control::ScreenLightController>(parent),
                        parent,
                        [this](sys::Timer &t) {
                            if (this->screenLightController->isLightOn()) {
                                if (backlightType == Type::BedsideLamp) {
                                    backlightType = Type::Frontlight;
                                }
                                this->screenLightController->processRequest(screen_light_control::Action::turnOff);
                            }
                        })
    {}

    void Handler::init()
    {
        using namespace screen_light_control;
        settings->registerValueChange(settings::Brightness::brightnessLevel, [&](const std::string &value) {
            ConstLinearProgressModeParameters params{utils::getNumericValue<float>(value)};
            screenLightController->processRequest(Action::setAutomaticModeParameters, Parameters(params));
        });

        settings->registerValueChange(settings::Brightness::autoMode, [&](const std::string &value) {
            onDemandModeOn = utils::getNumericValue<bool>(value);
        });
    }

    void Handler::handleKeyPressed([[maybe_unused]] int key)
    {
        handleScreenLightRefresh(key);
    }

    void Handler::handleScreenLightRefresh([[maybe_unused]] int key)
    {
        auto controller = getScreenLightControl();
        auto timer      = getScreenLightTimer();

        if (key != static_cast<int>(KeyMap::Frontlight) && backlightType != Type::BedsideLamp) {
            if (controller->isLightOn()) {
                timer->restart(timers::screenLightTimerHoldTimeout);
            }
            else if (!onDemandModeOn && !ignoreKeypress) {
                setKeyPressedModeFrontlightOn();
                controller->processRequest(screen_light_control::Action::turnOn);
                timer->restart(timers::screenLightTimerHoldTimeout);
            }
        }
    }

    void Handler::handleScreenLight(Type type)
    {
        const auto controller = getScreenLightControl();
        const auto timer      = getScreenLightTimer();

        auto lightTime = std::chrono::seconds(0);

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
            lightTime = timers::screenLightBedsideLampTimeout;
            break;
        }

        if (controller->isLightOn() && type == Type::Frontlight) {
            setKeyPressedModeFrontlightOff();
            controller->processRequest(screen_light_control::Action::turnOff);
            timer->stop();
        }
        else if (!controller->isLightOn()) {
            setKeyPressedModeFrontlightOn();
            controller->processRequest(screen_light_control::Action::turnOn);
            timer->restart(lightTime);
        }
        backlightType = type;
    }

    void Handler::processScreenRequest(screen_light_control::Action action,
                                       const screen_light_control::Parameters &params)
    {
        handleScreenLightSettings(action, params);
        getScreenLightControl()->processRequest(action, params);
    }

    void Handler::handleScreenLightSettings(screen_light_control::Action action,
                                            const screen_light_control::Parameters &params)
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

    void Handler::onScreenLightTurnedOn()
    {
        if (backlightMode == BacklightMode::WithTimer && backlightType != Type::BedsideLamp) {
            startScreenLightTimer();
        }
    }

    void Handler::setKeyPressedModeFrontlightOn()
    {
        using namespace screen_light_control;
        auto brightnessLevel = utils::getNumericValue<float>(getValue(settings::Brightness::brightnessLevel));
        screen_light_control::ConstLinearProgressModeParameters params{brightnessLevel};
        screenLightController->processRequest(Action::setAutomaticModeParameters, Parameters(params));
    }

    void Handler::setKeyPressedModeFrontlightOff()
    {
        using namespace screen_light_control;
        screenLightController->processRequest(Action::setAutomaticModeParameters,
                                              Parameters(screen_light_control::ConstLinearProgressModeParameters{0}));
    }
} // namespace backlight
