// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Constants.hpp"

#include <MessageType.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <bsp/common.hpp>
#include <bsp/keyboard/key_codes.hpp>
#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <screen-light-control/ScreenLightControl.hpp>
#include <vibra/Vibra.hpp>

#include <service-db/DBServiceName.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>

class WorkerEvent;

class EventManager : public sys::Service
{
  private:
    void handleMinuteUpdate(time_t timestamp);
    bool processKeypadBacklightRequest(bsp::keypad_backlight::Action act);
    bool processVibraRequest(bsp::vibrator::Action act, sys::ms RepetitionTime = static_cast<sys::ms>(1000));

    std::shared_ptr<settings::Settings> settings;

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

    std::unique_ptr<screen_light_control::ScreenLightControl> screenLightControl;
    std::unique_ptr<vibra_handle::Vibra> Vibra;

  public:
    EventManager(const std::string &name = service::name::evt_manager);
    ~EventManager();

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    void ProcessCloseReason(sys::CloseReason closeReason) override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

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
