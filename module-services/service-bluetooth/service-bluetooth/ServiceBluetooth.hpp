// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Constants.hpp"
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include "service-bluetooth/SettingsHolder.hpp"
#include <service-db/DBServiceName.hpp>
#include <service-audio/ServiceAudio.hpp>
#include <module-bluetooth/Bluetooth/CommandHandler.hpp>
#include "BluetoothMessage.hpp"
#include "ProfileManager.hpp"

#include <memory> // for unique_ptr

class BluetoothWorker;
namespace settings
{
    class Settings;
}

class ServiceBluetooth : public sys::Service
{

  public:
    ServiceBluetooth();
    ~ServiceBluetooth();

    virtual sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    virtual sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
    void sendWorkerCommand(bluetooth::Command command);
    QueueHandle_t workerQueue = nullptr;
    std::shared_ptr<bluetooth::SettingsHolder> settingsHolder;
    bluetooth::ProfileManager *profileManagerPtr = nullptr;
    void scanStartedCallback();
    void scanStoppedCallback();

  private:
    std::unique_ptr<BluetoothWorker> worker;
    bool enabledFromHarness = false;
};

namespace sys
{
    template <> struct ManifestTraits<ServiceBluetooth>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::bluetooth;
            manifest.dependencies = {service::name::db, service::name::audio};
            return manifest;
        }
    };
} // namespace sys
