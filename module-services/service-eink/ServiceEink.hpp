// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <EinkIncludes.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Timer.hpp>

#include <cstdint>
#include <string>

class ServiceEink : public sys::Service
{
  protected:
    // counts timer triggers from last self refresh
    uint32_t selfRefereshTriggerCount;
    // counts timer events from last temperature measurement
    uint32_t temperatureMeasurementTriggerCount;
    // counts trigger counts from last action that required eink to be powered on
    uint32_t powerOffTriggerCount;

    // number of timer triggers required to execute self refresh handler
    const uint32_t selfRefereshTriggerValue = 60;
    // number of timer triggers required to execute temperature measurement handler
    const uint32_t temperatureMeasurementTriggerValue = 5 * 60;
    // number of timer triggers from last action requiring power on eink to power down eink.
    const uint32_t powerOffTriggerValue = 3;

    // structure with recently loaded waveformdata
    EinkWaveFormSettings_t waveformSettings;

    EinkDisplayColorMode_e displayMode = EinkDisplayColorMode_e::EinkDisplayColorModeStandard;

    bool suspended = false;

    bool suspendInProgress  = false;
    bool shutdownInProgress = false;

    bool changeWaveform(EinkWaveforms_e Mode, const int32_t temperature);

    bool deepClearScreen(int8_t temperature);

    uint8_t einkRenderBuffer[600 * 480];
    bool deepRefresh         = false;

    sys::ms powerOffTime = 3000;
    sys::Timer powerOffTimer;

  public:
    ServiceEink(const std::string &name, std::string parent = "");
    ~ServiceEink();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;
};
