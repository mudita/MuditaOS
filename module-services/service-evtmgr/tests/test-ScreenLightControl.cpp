// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <screen-light-control/ScreenLightControl.hpp>
#include <screen-light-control/ControlFunctions.hpp>
#include <Service/Service.hpp>
#include <Service/Message.hpp>
#include <iostream>

class TestService : public sys::Service
{
  public:
    TestService() : sys::Service("test service")
    {}

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override
    {
        return nullptr;
    }

    sys::ReturnCodes InitHandler() override
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes DeinitHandler() override
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override
    {
        return sys::ReturnCodes::Success;
    }
};

TEST_CASE("ScreenLightControl")
{
    using namespace sevm::screen_light_control;
    TestService testService;
    init(&testService);

    SECTION("Automatic mode ramp an hysteresis test")
    {
        INFO("Setup automatic mode");
        Action act = Action::setAutomaticModeParameters;
        Parameters params;
        const unsigned int testRampTime                          = 500;
        params.rampTimeMS                                        = testRampTime;
        const bsp::eink_frontlight::BrightnessPercentage testVal = 100.0f;
        params.functionPoints       = functions::BrightnessFunction({{0.0f, testVal}, {100.0f, 0.0f}});
        const float hysteresis      = 10.0f;
        params.brightnessHysteresis = hysteresis;
        processRequest(act, params);

        INFO("Simple ramp test");
        // Timer callback cycles
        int cyclesPerRamp = testRampTime / 25;
        // Mock the measurement
        functions::calculateBrightness(0.0f);
        auto rampOut = 0.0f;
        for (int i = 0; i < cyclesPerRamp; ++i) {
            rampOut = functions::brightnessRampOut();
        }
        CHECK(rampOut == testVal);

        INFO("Hysteresis preventing the change");
        functions::calculateBrightness(9.5f);
        for (int i = 0; i < cyclesPerRamp; ++i) {
            rampOut = functions::brightnessRampOut();
        }
        // No chnage due to hysteresis
        CHECK(rampOut == testVal);

        INFO("Out of hysteresis");
        functions::calculateBrightness(110.0f);
        for (int i = 0; i < cyclesPerRamp; ++i) {
            rampOut = functions::brightnessRampOut();
        }
        CHECK(rampOut == 0.0f);
    }

    SECTION("Brightness function check")
    {
        INFO("Setup automatic mode and iterate through points");
        Action act = Action::setAutomaticModeParameters;
        Parameters params;
        const unsigned int testRampTime = 100;
        params.rampTimeMS               = testRampTime;
        params.functionPoints           = functions::BrightnessFunction({{50.0f, 50.0f},
                                                               {100.0f, 100.0f},
                                                               {150.0f, 100.0f},
                                                               {200.0f, 0.0f},
                                                               {250.0f, 0.0f},
                                                               {300.0f, 100.0f},
                                                               {350.0f, 50.0f}});
        const float hysteresis          = 10.0f;
        params.brightnessHysteresis     = hysteresis;
        processRequest(act, params);

        int cyclesPerRamp = testRampTime / 25;
        // Checking values between the points
        functions::BrightnessFunction testPoints = functions::BrightnessFunction({{0.0f, 50.0f},
                                                                                  {75.0f, 75.0f},
                                                                                  {125.0f, 100.0f},
                                                                                  {175.0f, 50.0f},
                                                                                  {225.0f, 0.0f},
                                                                                  {275.0f, 50.0f},
                                                                                  {325.0f, 75.0f},
                                                                                  {400.0f, 50.0f}});
        for (const auto &point : testPoints) {
            functions::calculateBrightness(point.first);
            auto rampOut = 0.0f;
            for (int i = 0; i < cyclesPerRamp; ++i) {
                rampOut = functions::brightnessRampOut();
            }
            INFO("Measurement:" << point.first << " Expected:" << point.second << " Actual:" << rampOut);
            CHECK(rampOut == point.second);
        }
    }

    deinit();
}
