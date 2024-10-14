// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "ScreenLightControl.hpp"

#include <Service/Service.hpp>
#include <Timers/TimerFactory.hpp>

#include <system/Constants.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>

namespace bell::screen_light_control
{
    ScreenLightController::ScreenLightController(sys::Service *parent)
        : cpuSentinel(std::make_shared<sys::CpuSentinel>(screenLightControlName, parent))
    {
        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
        parent->bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);

        controlTimer  = sys::TimerFactory::createPeriodicTimer(parent,
                                                              "LightControlTimer",
                                                              std::chrono::milliseconds{controlTimerIntervalMs},
                                                              [this](sys::Timer &) { controlTimerCallback(); });
        automaticMode = ScreenLightMode::Automatic;
    }

    ScreenLightController::~ScreenLightController()
    {
        disableTimers();
    }

    auto ScreenLightController::processRequest(Action action) -> void
    {
        processRequest(action, Parameters{});
    }

    auto ScreenLightController::processRequest(Action action, const Parameters &params) -> void
    {
        switch (action) {
        case Action::turnOff:
            turnOff();
            break;
        case Action::turnOn:
            turnOn(params.hasManualModeParams() ? std::optional<ManualModeParameters>(params.getManualModeParams())
                                                : std::nullopt);
            break;
        case Action::setAutomaticModeParameters:
            if (params.hasLinearProgressModeParams()) {
                setParameters(params.getLinearProgressModeParams());
            }
            else if (params.hasConstLinearProgressModeParams()) {
                setParameters(params.getConstLinearProgressModeParams());
            }
            break;
        default:
            break;
        }
    }

    auto ScreenLightController::controlTimerCallback() -> void
    {
        auto newBrightness = ::screen_light_control::functions::brightnessRampOut();
        bsp::eink_frontlight::setBrightness(newBrightness);

        if (::screen_light_control::functions::isRampTargetReached()) {
            if (!automaticModeFunctions.empty()) {
                setUpAutomaticFunction(getNextAutomaticFunction(), newBrightness);
            }
            else {
                if (newBrightness == minimalTargetBrightness) {
                    turnOff();
                    bsp::eink_frontlight::turnOff();
                }
                setBrightnessInstant(minimalTargetBrightness);
                disableTimers();
            }
        }
    }

    auto ScreenLightController::isLightOn() const noexcept -> bool
    {
        return lightOn;
    }

    auto ScreenLightController::isAutoModeOn() const noexcept -> bool
    {
        return automaticMode == ScreenLightMode::Automatic;
    }

    auto ScreenLightController::getBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage
    {
        return brightnessWhenOn;
    }

    auto ScreenLightController::enableTimers() -> void
    {
        controlTimer.start();
    }

    auto ScreenLightController::disableTimers() -> void
    {
        controlTimer.stop();
    }

    auto ScreenLightController::setParameters(const LinearProgressModeParameters &params) -> void
    {
        if (params.functions.empty()) {
            disableTimers();
            return;
        }
        setAutomaticModeFunctions(params.functions);

        if (lightOn && automaticMode == ScreenLightMode::Automatic) {
            disableTimers();
        }

        ::screen_light_control::functions::setHysteresis(params.brightnessHysteresis);
        setUpAutomaticFunction(getNextAutomaticFunction(), ::screen_light_control::functions::getRampState());
        setBrightnessInstant(getStandarizedRampTarget(params.startBrightnessValue));

        if (lightOn && automaticMode == ScreenLightMode::Automatic) {
            enableTimers();
        }
    }

    auto ScreenLightController::setAutomaticModeFunctions(
        const LinearProgressModeParameters::LinearFunctions &functions) -> void
    {
        automaticModeFunctions = LinearProgressModeParameters::LinearFunctions{functions.rbegin(), functions.rend()};
    }

    auto ScreenLightController::getNextAutomaticFunction() -> ::screen_light_control::functions::LinearProgressFunction
    {
        auto function = automaticModeFunctions.back();
        automaticModeFunctions.pop_back();
        return function;
    }

    auto ScreenLightController::setUpAutomaticFunction(
        ::screen_light_control::functions::LinearProgressFunction function,
        bsp::eink_frontlight::BrightnessPercentage currentBrightness) -> void
    {
        const auto rampTarget = getStandarizedRampTarget(function.target);
        ::screen_light_control::functions::setRampStep(
            std::abs(rampTarget - currentBrightness) /
            (static_cast<float>(function.duration.count()) / controlTimerIntervalMs));
        ::screen_light_control::functions::setRampTarget(rampTarget);
    }

    auto ScreenLightController::setParameters(const ConstLinearProgressModeParameters &params) -> void
    {
        setBrightnessInstant(getStandarizedRampTarget(params.targetBrightness));
    }

    auto ScreenLightController::turnOn(const std::optional<ManualModeParameters> &params) -> void
    {
        if (params.has_value()) {
            brightnessWhenOn = getStandarizedRampTarget(params->manualModeBrightness);
        }

        ::screen_light_control::functions::setRampTarget(brightnessWhenOn);
        bsp::eink_frontlight::turnOn();
        cpuSentinelKeepOn();
        enableTimers();
        lightOn = true;
    }

    auto ScreenLightController::setBrightnessInstant(bsp::eink_frontlight::BrightnessPercentage brightness) -> void
    {
        brightnessWhenOn = getStandarizedRampTarget(brightness);
        ::screen_light_control::functions::setRampTarget(brightnessWhenOn);
        ::screen_light_control::functions::setRampStep(rampStepPerMs * controlTimerIntervalMs);
    }

    auto ScreenLightController::turnOff() -> void
    {
        lightOn = false;
        setBrightnessInstant(minimalTargetBrightness);
        cpuSentinelRelease();
        enableTimers();
    }

    auto ScreenLightController::cpuSentinelKeepOn() -> void
    {
        cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_3);
    }

    auto ScreenLightController::cpuSentinelRelease() -> void
    {
        cpuSentinel->ReleaseMinimumFrequency();
    }

    auto ScreenLightController::getStandarizedRampTarget(bsp::eink_frontlight::BrightnessPercentage target)
        -> bsp::eink_frontlight::BrightnessPercentage
    {
        return std::max<bsp::eink_frontlight::BrightnessPercentage>(target, minimalTargetBrightness);
    }

    auto ScreenLightController::isFadeOutOngoing() -> bool
    {
        return false;
    }
} // namespace bell::screen_light_control
