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

#include "EinkScreen.hpp"

namespace eink
{
    class ServiceEink : public sys::Service
    {
      protected:
        EinkScreen screen;

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

        bool suspended = false;

        bool suspendInProgress  = false;
        bool shutdownInProgress = false;
        bool deepRefresh        = false;

        sys::ms powerOffTime = 3000;
        sys::Timer powerOffTimer;

      public:
        explicit ServiceEink(const std::string &name, std::string parent = {});

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

      private:
        void enterActiveMode();
        void suspend();

        sys::MessagePointer handleEinkModeChangedMessage(sys::Message *message);
        sys::MessagePointer handleEinkDMATransfer(sys::Message *message);
        sys::MessagePointer handleImageMessage(sys::Message *message);
        sys::MessagePointer handleStateRequest(sys::Message *messge);
        sys::MessagePointer handleTemperatureUpdate(sys::Message *);
    };
} // namespace eink
