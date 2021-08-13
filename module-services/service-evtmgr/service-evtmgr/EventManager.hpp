﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Constants.hpp"

#include <common_data/RawKey.hpp>
#include <MessageType.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <service-db/DBServiceName.hpp>
#include <Timers/TimerHandle.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <service-db/Settings.hpp>

class WorkerEvent;

class EventManager : public sys::Service
{
  private:
    static constexpr auto stackDepth = 4096;
    void handleMinuteUpdate(time_t timestamp);

    void processRTCFromTmRequest(struct tm &newTime);
    void processRTCFromTimestampRequest(time_t &newTime);
    void processTimezoneRequest(const std::string &timezone);

    sys::TimerHandle loggerTimer;

  protected:
    virtual void handleKeyEvent(sys::Message *msg);

    std::shared_ptr<settings::Settings> settings;
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

  public:
    explicit EventManager(const std::string &name = service::name::evt_manager);
    ~EventManager();

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    void ProcessCloseReason(sys::CloseReason closeReason) override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    /// @return: < 0 - An error occurred during log flush
    /// @return:   0 - Log file reached max size
    /// @return:   1 - Logs flushed successflully
    int dumpLogsToFile();

    /**
     * @brief Sends request to application manager to switch from current application to specific window in application
     * with specified name .
     */
    static bool messageSetApplication(sys::Service *sender, const std::string &applicationName);
};
