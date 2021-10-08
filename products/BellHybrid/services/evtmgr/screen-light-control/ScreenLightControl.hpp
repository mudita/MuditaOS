// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControlParameters.hpp>

#include <Timers/TimerHandle.hpp>

#include <memory>

namespace sys
{
    class Service;
} // namespace sys

/// Screen light control algorithm. Automatic/Manual mode of operation.
/// Processing of ambient light sensor input to screen brightness output.
namespace bell::screen_light_control
{
    /// Control screen light and keeps it's current state
    class ScreenLightController : public ::screen_light_control::ScreenLightController
    {
      public:
        using Action                       = ::screen_light_control::Action;
        using Parameters                   = ::screen_light_control::Parameters;
        using ScreenLightMode              = ::screen_light_control::ScreenLightMode;
        using ManualModeParameters         = ::screen_light_control::ManualModeParameters;
        using LinearProgressModeParameters = ::screen_light_control::LinearProgressModeParameters;

        explicit ScreenLightController(sys::Service *parent);
        ~ScreenLightController() override;

        void processRequest(Action action) override;
        void processRequest(Action action, const Parameters &params) override;

        [[nodiscard]] auto isLightOn() const noexcept -> bool override;
        [[nodiscard]] bool isAutoModeOn() const noexcept override;
        [[nodiscard]] auto getBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage override;

      private:
        void controlTimerCallback();

        void enableTimers();
        void disableTimers();

        void setParameters(const LinearProgressModeParameters &params);
        void setParameters(ManualModeParameters params);
        void setManualBrightnessLevel(bsp::eink_frontlight::BrightnessPercentage brightness);

        void setAutomaticModeFunctions(const LinearProgressModeParameters::LinearFunctions &functions);
        ::screen_light_control::functions::LinearProgressFunction getNextAutomaticFunction();
        void setUpAutomaticFunction(::screen_light_control::functions::LinearProgressFunction function,
                                    bsp::eink_frontlight::BrightnessPercentage currentBrightness);

        void turnOff();
        void turnOn(const std::optional<ManualModeParameters> &params = std::nullopt);

        void enableAutomaticMode();
        void disableAutomaticMode();

        static constexpr inline auto CONTROL_TIMER_MS = 25;

        sys::TimerHandle controlTimer;
        bool lightOn                                               = false;
        ScreenLightMode automaticMode                              = ScreenLightMode::Manual;
        bsp::eink_frontlight::BrightnessPercentage brightnessValue = 0.0;
        LinearProgressModeParameters::LinearFunctions automaticModeFunctions;
    };
} // namespace bell::screen_light_control
