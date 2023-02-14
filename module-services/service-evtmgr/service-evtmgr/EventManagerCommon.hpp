// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Constants.hpp"

#include <hal/key_input/RawKey.hpp>
#include <MessageType.hpp>
#include <system/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <service-db/DBServiceName.hpp>
#include <Timers/TimerHandle.hpp>
#include <SystemManager/CpuSentinel.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <service-db/Settings.hpp>

class WorkerEventCommon;

class EventManagerCommon : public sys::Service
{
  public:
    using LogDumpFunction = std::function<int()>;

    explicit EventManagerCommon(LogDumpFunction logDumpFunction, const std::string &name = service::name::evt_manager);
    ~EventManagerCommon() override;

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler();

    sys::ReturnCodes DeinitHandler() override;

    void ProcessCloseReasonHandler(sys::CloseReason closeReason) override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    /**
     * @brief Sends request to application manager to switch from current application to specific window in application
     * with specified name .
     */
    static bool messageSetApplication(sys::Service *sender, const std::string &applicationName);

  private:
    static constexpr auto stackDepth = 1024 * 4;
    void handleMinuteUpdate(time_t timestamp);

    void processRTCFromTmRequest(struct tm &newTime);
    void processRTCFromTimestampRequest(time_t &newTime);
    void processTimezoneRequest(const std::string &timezone);

    LogDumpFunction logDumpFunction;

    /// @return: < 0 - error occured during log flush
    /// @return:   0 - log flush did not happen
    /// @return:   1 - log flush successflul
    int dumpLogsToFile();

  protected:
    std::function<void(const time_t)> onMinuteTick;
    virtual void handleKeyEvent(sys::Message *msg);
    virtual void initProductEvents();
    virtual auto createEventWorker() -> std::unique_ptr<WorkerEventCommon> = 0;

    std::shared_ptr<settings::Settings> settings;
    std::unique_ptr<WorkerEventCommon> EventWorker;
    std::shared_ptr<sys::TimedCpuSentinel> cpuSentinel;
    // application where key events are sent. This is also only application that is allowed to change keyboard long
    // press settings.
    std::string targetApplication;
    // alarm timestamp in seconds from midnight
    uint32_t alarmTimestamp;
    // ID of alarm waiting to trigger
    uint32_t alarmID;
};
