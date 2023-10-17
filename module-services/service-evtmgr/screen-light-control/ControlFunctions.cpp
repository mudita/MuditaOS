// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ControlFunctions.hpp"
#include <cmath>

namespace screen_light_control::functions
{
    namespace
    {
        struct FunctionSection
        {
            float xBound;
            float a;
            float b;
        };

        std::vector<FunctionSection> linearFunction;
        float rampStep;
        float hysteresis;

        bsp::eink_frontlight::BrightnessPercentage rampTarget;
        bsp::eink_frontlight::BrightnessPercentage rampState;
        bool rampTargetReached = false;
    } // namespace

    void calculateBrightness(bsp::light_sensor::IlluminanceLux measurement)
    {
        for (const auto &section : linearFunction) {
            if (measurement < section.xBound) {
                rampTarget = section.a * measurement + section.b;
                return;
            }
        }

        if (linearFunction.empty()) {
            rampTarget = 0.0f;
            return;
        }
        rampTarget = linearFunction.back().xBound * linearFunction.back().a + linearFunction.back().b;
    }

    bsp::eink_frontlight::BrightnessPercentage brightnessRampOut()
    {
        if (rampTargetReached && std::abs(rampTarget - rampState) > hysteresis) {
            rampTargetReached = false;
        }

        if (!rampTargetReached) {
            if (rampState < rampTarget) {
                rampState += rampStep;
                if (rampState >= rampTarget) {
                    rampState         = rampTarget;
                    rampTargetReached = true;
                }
            }
            else if (rampState > rampTarget) {
                rampState -= rampStep;
                if (rampState <= rampTarget) {
                    rampState         = rampTarget;
                    rampTargetReached = true;
                }
            }
            else {
                rampTargetReached = true;
            }
        }

        return rampState;
    }

    void setRampStep(float step)
    {
        rampStep = step;
    }

    void setHysteresis(float hyst)
    {
        hysteresis = hyst;
    }

    void setFunctionFromPoints(const BrightnessFunction &points)
    {
        if (!points.empty()) {
            linearFunction.clear();
            for (unsigned int i = 0; i < points.size(); ++i) {
                FunctionSection section;
                section.xBound = points[i].first;
                if (i == 0) {
                    section.a = 0.0f;
                    section.b = points[i].second;
                }
                else {
                    section.a = (points[i - 1].second - points[i].second) / (points[i - 1].first - points[i].first);
                    section.b = points[i - 1].second - section.a * points[i - 1].first;
                }
                linearFunction.push_back(section);
            }
        }
    }

    void setRampState(float state)
    {
        rampState = state;
    }

    void setRampTarget(bsp::eink_frontlight::BrightnessPercentage value)
    {
        rampTarget = value;
    }

    void resetRampToTarget()
    {
        rampState = rampTarget;
    }

    bool isRampTargetReached()
    {
        return rampTargetReached;
    }

    bsp::eink_frontlight::BrightnessPercentage getRampState()
    {
        return rampState;
    }
} // namespace screen_light_control::functions
