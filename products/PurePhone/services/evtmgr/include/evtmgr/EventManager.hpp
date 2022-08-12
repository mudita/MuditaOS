// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-evtmgr/EventManagerCommon.hpp>

#include "BacklightHandler.hpp"
#include "UserActivityHandler.hpp"
#include <bsp/vibrator/vibrator.hpp>
#include <vibra/Vibra.hpp>

class EventManager : public EventManagerCommon
{
  public:
    explicit EventManager(LogDumpFunction logDumpFunction = nullptr,
                          const std::string &name         = service::name::evt_manager)
        : EventManagerCommon(logDumpFunction, name), vibrator(std::make_unique<vibra_handle::Vibra>(this)),
          backlightHandler(settings, this), userActivityHandler(std::make_shared<sys::CpuSentinel>(name, this), this)
    {}

  private:
    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
    void toggleTorchOnOff();
    void toggleTorchOff();
    void toggleTorchColor();
    void ProcessCloseReason(sys::CloseReason closeReason) override;
    void handleKeyEvent(sys::Message *msg) override;
    void handleKeyMoveEvent(RawKey key);

    void processVibratorRequest(bsp::vibrator::Action act,
                                std::chrono::milliseconds RepetitionTime = std::chrono::milliseconds{1000});
    void processVibratorLevel(unsigned int vibrationLevel);

    void initProductEvents() final;
    auto createEventWorker() -> std::unique_ptr<WorkerEventCommon> final;

    std::unique_ptr<vibra_handle::Vibra> vibrator;
    backlight::Handler backlightHandler;
    evm::UserActivityHandler userActivityHandler;
};

namespace sys
{
    template <> struct ManifestTraits<EventManager>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::evt_manager;
            manifest.dependencies = {/* service::name::db */};
            return manifest;
        }
    };
} // namespace sys
