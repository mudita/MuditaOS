// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/Common.hpp>
#include <EinkIncludes.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Timer.hpp>

#include <cstdint>
#include <string>

class ServiceEink : public sys::Service
{
    gui::Size screen        = {480, 600};
    gui::Point pointTopLeft = {0, 0};
    std::unique_ptr<uint8_t[]> einkRenderBuffer;

  protected:
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

    bool deepRefresh         = false;

  public:
    ServiceEink(const std::string &name, std::string parent = "");
    ~ServiceEink() override;

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

  private:
    sys::Message_t handleEinkDMATransfer(sys::Message *message);
    sys::Message_t handleImageMessage(sys::Message *message);
    sys::Message_t handleStateRequest(sys::Message *messge);
    sys::Message_t handleTemperatureUpdate(sys::Message *);
};
