// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <screen-light-control/ControlFunctions.hpp>

TEST_CASE("ScreenLightControlFunctions")
{
    using namespace screen_light_control::functions;
    constexpr auto controlTimerMS = 25;

    SECTION("Ramp an hysteresis test")
    {
        INFO("Setup");
        const unsigned int testRampTime                          = 500;
        const bsp::eink_frontlight::BrightnessPercentage testVal = 100.0f;
        screen_light_control::functions::BrightnessFunction functionPoints =
            BrightnessFunction({{0.0f, testVal}, {100.0f, 0.0f}});
        const float hysteresis = 10.0f;

        setRampStep(100.0f * (static_cast<float>(controlTimerMS) / static_cast<float>(testRampTime)));
        setHysteresis(hysteresis);
        setFunctionFromPoints(functionPoints);

        INFO("Simple ramp test");
        // Timer callback cycles
        int cyclesPerRamp = testRampTime / controlTimerMS;
        // Mock the measurement
        calculateBrightness(0.0f);
        auto rampOut = 0.0f;
        for (int i = 0; i < cyclesPerRamp; ++i) {
            rampOut = brightnessRampOut();
        }
        CHECK(rampOut == testVal);

        INFO("Hysteresis preventing the change");
        calculateBrightness(9.5f);
        for (int i = 0; i < cyclesPerRamp; ++i) {
            rampOut = brightnessRampOut();
        }
        CHECK(rampOut == testVal);

        INFO("Out of hysteresis");
        calculateBrightness(110.0f);
        for (int i = 0; i < cyclesPerRamp; ++i) {
            rampOut = brightnessRampOut();
        }
        CHECK(rampOut == 0.0f);
    }

    SECTION("Brightness function check")
    {
        INFO("Setup and iterate through points");
        const unsigned int testRampTime   = 100;
        BrightnessFunction functionPoints = BrightnessFunction({{50.0f, 50.0f},
                                                                {100.0f, 100.0f},
                                                                {150.0f, 100.0f},
                                                                {200.0f, 0.0f},
                                                                {250.0f, 0.0f},
                                                                {300.0f, 100.0f},
                                                                {350.0f, 50.0f}});
        const float hysteresis            = 10.0f;

        setRampStep(100.0f * (static_cast<float>(controlTimerMS) / static_cast<float>(testRampTime)));
        setHysteresis(hysteresis);
        setFunctionFromPoints(functionPoints);

        int cyclesPerRamp = testRampTime / controlTimerMS;
        // Checking values between the points
        BrightnessFunction testPoints = BrightnessFunction({{0.0f, 50.0f},
                                                            {75.0f, 75.0f},
                                                            {125.0f, 100.0f},
                                                            {175.0f, 50.0f},
                                                            {225.0f, 0.0f},
                                                            {275.0f, 50.0f},
                                                            {325.0f, 75.0f},
                                                            {400.0f, 50.0f}});
        for (const auto &point : testPoints) {
            calculateBrightness(point.first);
            auto rampOut = 0.0f;
            for (int i = 0; i < cyclesPerRamp; ++i) {
                rampOut = brightnessRampOut();
            }
            INFO("Measurement:" << point.first << " Expected:" << point.second << " Actual:" << rampOut);
            CHECK(rampOut == point.second);
        }
    }
}
