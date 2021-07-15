// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Constants.hpp"

#include <backlight-handler/BacklightHandler.hpp>
#include <bsp/vibrator/vibrator.hpp>
#include <MessageType.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>
#include <Service/Worker.hpp>
#include <service-db/DBServiceName.hpp>
#include <common_data/RawKey.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>

class WorkerEvent;

namespace vibra_handle
{
    class Vibra;
} // namespace vibra_handle

class EventManager : public sys::Service
{
  private:
    static constexpr auto stackDepth = 4096;
    void handleKeyEvent(sys::Message *msg);
    void handleKeyMoveEvent(RawKey key);
    void handleMinuteUpdate(time_t timestamp);
    bool processVibraRequest(bsp::vibrator::Action act,
                             std::chrono::milliseconds RepetitionTime = std::chrono::milliseconds{1000});
    void toggleTorchOnOff();
    void toggleTorchColor();

    void processRTCFromTmRequest(struct tm &newTime);
    void processRTCFromTimestampRequest(time_t &newTime);
    void processTimezoneRequest(const std::string &timezone);

    std::shared_ptr<settings::Settings> settings;
    sys::TimerHandle loggerTimer;

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

    std::unique_ptr<vibra_handle::Vibra> Vibra;
    backlight::Handler backlightHandler;

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

namespace sys
{
    template <> struct ManifestTraits<EventManager>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::evt_manager;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys
