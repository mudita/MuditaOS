// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <service-evtmgr/ScreenLightControl.hpp>
#include <Service/Service.hpp>
#include <Service/Message.hpp>
#include <iostream>

namespace bsp::eink_frontlight
{
    extern bool isOn;
    extern BrightnessPercentage actualBrightness;
} // namespace bsp::eink_frontlight

namespace bsp::light_sensor
{
    extern bool isOn;
    extern IlluminanceLux measurement;
} // namespace bsp::light_sensor

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

    SECTION("Turn on and off")
    {
        INFO("Check if system off");
        REQUIRE(!bsp::eink_frontlight::isOn);
        REQUIRE(!bsp::light_sensor::isOn);

        INFO("Command TurnOn and check");
        Action act = Action::turnOn;
        Parameters defaultParams;
        processRequest(act, defaultParams);
        REQUIRE(bsp::eink_frontlight::isOn);
        REQUIRE(bsp::light_sensor::isOn);

        INFO("Command TurnOff and check");
        act = Action::turnOff;
        processRequest(act, defaultParams);
        REQUIRE(!bsp::eink_frontlight::isOn);
        REQUIRE(!bsp::light_sensor::isOn);
    }

    SECTION("Set manual brigthness")
    {
        INFO("Try change brightness in manual mode");
        Action act = Action::setManualModeBrightness;
        Parameters params;
        const bsp::eink_frontlight::BrightnessPercentage testVal = 21.37f;
        params.manualModeBrightness                              = testVal;
        processRequest(act, params);
        REQUIRE(bsp::eink_frontlight::actualBrightness == testVal);
    }

    SECTION("Automatic mode ramp an hsyteresis test")
    {
        INFO("Setup automatic mode");
        Action act = Action::setAutomaticModeParameters;
        Parameters params;
        const unsigned int testRampTime                          = 500;
        params.rampTimeMS                                        = testRampTime;
        const bsp::eink_frontlight::BrightnessPercentage testVal = 100.0f;
        params.functionPoints       = BrightnessFunction({{0.0f, testVal}, {100.0f, 0.0f}});
        const float hysteresis      = 10.0f;
        params.brightnessHysteresis = hysteresis;
        processRequest(act, params);

        INFO("Simple ramp test");
        // Timer callback cycles
        int cyclesPerRamp = testRampTime / 25;
        // Mock the measurement
        bsp::light_sensor::measurement = 0.0f;
        readoutTimerCallback();
        for (int i = 0; i < cyclesPerRamp; ++i) {
            controlTimerCallback();
        }
        REQUIRE(bsp::eink_frontlight::actualBrightness == testVal);

        INFO("Hysteresis preventing the change");
        bsp::light_sensor::measurement = 9.5f;
        readoutTimerCallback();
        for (int i = 0; i < cyclesPerRamp; ++i) {
            controlTimerCallback();
        }
        // No change due to hysteresis
        REQUIRE(bsp::eink_frontlight::actualBrightness == testVal);

        INFO("Out of hysteresis");
        bsp::light_sensor::measurement = 110.0f;
        readoutTimerCallback();
        for (int i = 0; i < cyclesPerRamp; ++i) {
            controlTimerCallback();
        }
        REQUIRE(bsp::eink_frontlight::actualBrightness == 0);
    }

    SECTION("Brigthness funtcion check")
    {
        INFO("Setup automatic mode and iterate through points");
        Action act = Action::setAutomaticModeParameters;
        Parameters params;
        const unsigned int testRampTime = 100;
        params.rampTimeMS               = testRampTime;
        params.functionPoints           = BrightnessFunction({{50.0f, 50.0f},
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
        BrightnessFunction testPoints = BrightnessFunction({{0.0f, 50.0f},
                                                            {75.0f, 75.0f},
                                                            {125.0f, 100.0f},
                                                            {175.0f, 50.0f},
                                                            {225.0f, 0.0f},
                                                            {275.0f, 50.0f},
                                                            {325.0f, 75.0f},
                                                            {400.0f, 50.0f}});
        for (const auto &point : testPoints) {
            bsp::light_sensor::measurement = point.first;
            readoutTimerCallback();
            for (int i = 0; i < cyclesPerRamp; ++i) {
                controlTimerCallback();
            }
            INFO("Measurement:" << point.first << " Expected:" << point.second
                                << " Actual:" << bsp::eink_frontlight::actualBrightness);
            REQUIRE(bsp::eink_frontlight::actualBrightness == point.second);
        }
    }

    deinit();
}
