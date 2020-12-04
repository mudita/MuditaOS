// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/LightControl.hpp"

namespace sevm::light_control
{
    namespace
    {
        constexpr inline auto CONTROL_TIMER_MS = 20;
        constexpr inline auto READOUT_TIMER_MS = 500;
        std::unique_ptr<sys::Timer> controlTimer;
        std::unique_ptr<sys::Timer> readoutTimer;

        constexpr inline auto MAX_BRIGHTNESS_PERCENT = 80.0f;
        constexpr inline auto MIN_BRIGHTNESS_PERCENT = 20.0f;
        constexpr inline auto DIMMING_THRESHOLD_LUX  = 50.0f;
        constexpr inline auto MIN_BRIGHTNESS_LUX     = 500.0f;
        constexpr inline auto RAMP_TIME_0_TO_100_MS  = 1500.0f; // 1.5s
        constexpr inline auto RAMP_STEP              = 100.0f * (CONTROL_TIMER_MS / RAMP_TIME_0_TO_100_MS);

        constexpr inline auto BRIGHTNESS_CHANGE_SENSITIVITY = 10.0f;

        float brightnessRampTarget;
        float brightnessRampState;
        bool rampTargetReached = false;

        constexpr float calculateBrightness(bsp::light_sensor::IlluminanceLux measurement)
        {
            if (measurement < DIMMING_THRESHOLD_LUX) {
                return MAX_BRIGHTNESS_PERCENT;
            }
            else if (measurement < MIN_BRIGHTNESS_LUX) {
                constexpr auto a =
                    (MAX_BRIGHTNESS_PERCENT - MIN_BRIGHTNESS_PERCENT) / (DIMMING_THRESHOLD_LUX - MIN_BRIGHTNESS_LUX);
                constexpr auto b = MAX_BRIGHTNESS_PERCENT - a * DIMMING_THRESHOLD_LUX;
                return a * measurement + b;
            }

            return MIN_BRIGHTNESS_PERCENT;
        }

        bsp::eink_frontlight::BrightnessPercentage brightnessRampOut()
        {
            if (rampTargetReached &&
                std::abs(brightnessRampTarget - brightnessRampState) > BRIGHTNESS_CHANGE_SENSITIVITY) {
                rampTargetReached = false;
                LOG_DEBUG("out of hysteresis");
            }

            if (!rampTargetReached) {
                if (brightnessRampState < brightnessRampTarget) {
                    brightnessRampState += RAMP_STEP;
                    if (brightnessRampState > brightnessRampTarget) {
                        brightnessRampState = brightnessRampTarget;
                        rampTargetReached   = true;
                        LOG_DEBUG("ramp taget reached");
                    }
                }
                else if (brightnessRampState > brightnessRampTarget) {
                    brightnessRampState -= RAMP_STEP;
                    if (brightnessRampState < brightnessRampTarget) {
                        brightnessRampState = brightnessRampTarget;
                        rampTargetReached   = true;
                        LOG_DEBUG("ramp taget reached");
                    }
                }
            }

            return static_cast<bsp::eink_frontlight::BrightnessPercentage>(brightnessRampState);
        }
    } // namespace

    void init(sys::Service *parent)
    {
        controlTimer = std::make_unique<sys::Timer>("LightControlTimer", parent, CONTROL_TIMER_MS);
        controlTimer->connect([&](sys::Timer &) { controlTimerCallback(); });

        readoutTimer = std::make_unique<sys::Timer>("LightSensorReadoutTimer", parent, READOUT_TIMER_MS);
        readoutTimer->connect([&](sys::Timer &) { readoutTimerCallback(); });
    }

    void deinit()
    {
        controlTimer->stop();
        readoutTimer->stop();
    }

    bool processRequest(LightControlAction request, unsigned int value = 0)
    {
        switch (request) {
        case (LightControlAction::turnOff):
            bsp::keypad_backlight::shutdown();
            bsp::eink_frontlight::turnOff();
            bsp::light_sensor::standby();
            break;
        case (LightControlAction::turnOn):
            bsp::keypad_backlight::turnOnAll();
            bsp::eink_frontlight::turnOn();
            bsp::light_sensor::wakeup();
            break;
        case (LightControlAction::enableAutomaticMode):
            controlTimer->start();
            readoutTimer->start();
            break;
        case (LightControlAction::disableAutomaticMode):
            controlTimer->stop();
            readoutTimer->stop();
            break;
        case (LightControlAction::setScreenBrightness):
            bsp::eink_frontlight::setBrightness(value);
            break;
        }

        return true;
    }

    void controlTimerCallback()
    {
        auto out = brightnessRampOut();
        // LOG_DEBUG("ramp out: %d", out);
        bsp::eink_frontlight::setBrightness(out);
    }

    void readoutTimerCallback()
    {
        float lightMeasurement = bsp::light_sensor::readout();
        brightnessRampTarget   = calculateBrightness(lightMeasurement);
        LOG_DEBUG(
            "measurement: %d , bright: %d", static_cast<int>(lightMeasurement), static_cast<int>(brightnessRampTarget));
    }

} // namespace sevm::light_control
