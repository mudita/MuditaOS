﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-bluetooth/Constants.hpp>
#include <system/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include "Service/Mailbox.hpp"
#include "service-bluetooth/SettingsHolder.hpp"
#include "service-bluetooth/WorkerLock.hpp"
#include "service-bluetooth/ServiceBluetoothDependencies.hpp"
#include <service-db/DBServiceName.hpp>
#include <service-audio/ServiceAudio.hpp>
#include <module-bluetooth/Bluetooth/CommandHandler.hpp>
#include "ProfileManager.hpp"
#include "BluetoothDevicesModel.hpp"
#include <SystemManager/CpuSentinel.hpp>
#include <Timers/TimerHandle.hpp>

#include <memory> // for unique_ptr

namespace message::bluetooth
{
    class ResponseAuthenticate;
}
class BluetoothWorker;

namespace settings
{
    class Settings;
}

namespace sdesktop
{
    class Event;
    namespace developerMode
    {
        class DeveloperModeRequest;
    } // namespace developerMode
} // namespace sdesktop

namespace message::bluetooth
{
    class RequestBondedDevices;
    class RequestStatus;
    class SetStatus;
    class Unpair;
    class RequestDeviceName;
    class SetDeviceName;
    class Connect;
    class ConnectResult;
    class Disconnect;
    class DisconnectResult;
    class A2DPVolume;
    class HSPVolume;
    class HFPVolume;
    class RequestStatusIndicatorData;
} // namespace message::bluetooth

namespace sevm
{
    class BatteryStatusChangeMessage;
}

namespace cellular
{
    class CallEndedNotification;
    class CallMissedNotification;
    class CallStartedNotification;
    class CallOutgoingAccepted;
    class IncomingCallMessage;
    class CallerIdMessage;
    class SignalStrengthUpdateNotification;
    class CurrentOperatorNameNotification;
    class NetworkStatusUpdateNotification;
    class CallActiveNotification;
} // namespace cellular

class ServiceBluetooth : public sys::Service
{
  public:
    ServiceBluetooth();
    ~ServiceBluetooth();

    virtual sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    void ProcessCloseReason(sys::CloseReason closeReason) override;
    virtual sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

    void sendWorkerCommand(std::unique_ptr<bluetooth::event::Base> command);

    void handleTurnOff();
    void handleTurnOn();

    std::shared_ptr<Mailbox<bluetooth::Command, QueueHandle_t, WorkerLock>> workerQueue;
    std::shared_ptr<bluetooth::SettingsHolder> settingsHolder;
    bluetooth::BaseProfileManager *profileManagerPtr = nullptr;

  private:
    std::unique_ptr<BluetoothWorker> worker;
    std::shared_ptr<sys::CpuSentinel> cpuSentinel;
    sys::TimerHandle connectionTimeoutTimer, btRestartTimer;
    std::shared_ptr<BluetoothDevicesModel> bluetoothDevicesModel{};

    void startTimeoutTimer();
    void stopTimeoutTimer();

    template <typename T>
    auto connectHandler() -> bool
    {
        return connect(typeid(T), [&](sys::Message *msg) { return handle(static_cast<T *>(msg)); });
    }

    [[nodiscard]] auto handle(message::bluetooth::RequestBondedDevices *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::RequestStatus *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::SetStatus *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(BluetoothPairMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(BluetoothPairResultMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::Unpair *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::RequestDeviceName *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::SetDeviceName *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::Connect *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::ConnectResult *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::Disconnect *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::DisconnectResult *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(BluetoothMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(BluetoothAddrMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(sdesktop::developerMode::DeveloperModeRequest *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(BluetoothAudioStartMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::A2DPVolume *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::HSPVolume *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::HFPVolume *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::ResponseAuthenticatePin *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::ResponseAuthenticatePasskey *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::ResponseAuthenticatePairCancel *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::RequestStatusIndicatorData *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(cellular::CallerIdMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(cellular::IncomingCallMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(cellular::CallEndedNotification *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(cellular::CallMissedNotification *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(cellular::CallStartedNotification *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(cellular::CallOutgoingAccepted *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(cellular::SignalStrengthUpdateNotification *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(cellular::CurrentOperatorNameNotification *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(cellular::NetworkStatusUpdateNotification *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(sevm::BatteryStatusChangeMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(cellular::CallActiveNotification *msg) -> std::shared_ptr<sys::Message>;
};

namespace sys
{
    template <>
    struct ManifestTraits<ServiceBluetooth>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::bluetooth;
            manifest.dependencies = sys::dependencies::getDependenciesFor<ServiceBluetooth>();
            return manifest;
        }
    };
} // namespace sys
