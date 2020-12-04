// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <bsp/common.hpp>
#include <bsp/keyboard/key_codes.hpp>
#include "LightControl.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>

class WorkerEvent;

class EventManager : public sys::Service
{
  private:
    void HandleAlarmTrigger(sys::DataMessage *msgl);
    void GetNextAlarmTimestamp(time_t timestamp);

  protected:
    std::unique_ptr<WorkerEvent> EventWorker;
    // application where key events are sent. This is also only application that is allowed to change keyboard long
    // press settings.
    std::string targetApplication;
    // alarm timestamp in seconds from midnight
    uint32_t alarmTimestamp;
    // ID of alarm waiting to trigger
    uint32_t alarmID;
    // flag set when there is no alarm to trigger in current day
    bool alarmDBEmpty = false;
    // flag set when there is alarm to handle
    bool alarmIsValid = false;
    // flag informs about suspend/resume status
    bool suspended = false;

  public:
    EventManager(const std::string &name);
    ~EventManager();

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    /**
     * @brief Sends request to application manager to switch from current application to specific window in application
     * with specified name .
     */
    static bool messageSetApplication(sys::Service *sender, const std::string &applicationName);
};
