// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/switches/LatchStatusRequest.hpp>
#include <service-evtmgr/EventManagerCommon.hpp>

#include "backlight-handler/BacklightHandler.hpp"
#include "user-activity-handler/UserActivityHandler.hpp"

class KeySequenceMgr;

namespace hal::temperature
{
    class AbstractTemperatureSource;
}

class EventManager : public EventManagerCommon
{
  public:
    explicit EventManager(LogDumpFunction logDumpFunction = nullptr,
                          const std::string &name         = service::name::evt_manager);

  private:
    void handleKeyEvent(sys::Message *msg) override;
    sys::ReturnCodes InitHandler() final;
    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
    void initProductEvents() final;
    auto createEventWorker() -> std::unique_ptr<WorkerEventCommon> final;
    void buildKeySequences();
    std::shared_ptr<hal::temperature::AbstractTemperatureSource> temperatureSource;
    backlight::Handler backlightHandler;
    evm::UserActivityHandler userActivityHandler;

    std::shared_ptr<KeySequenceMgr> keySequenceMgr;

    sevm::LatchStatus latchStatus{};
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
