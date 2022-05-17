// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-bluetooth/ServiceBluetooth.hpp"
#include "service-bluetooth/BluetoothMessage.hpp"

#include <Bluetooth/BluetoothWorker.hpp>
#include <interface/profiles/Profile.hpp>
#include <MessageType.hpp>
#include <Service/Service.hpp>
#include <Service/Message.hpp>
#include <service-db/Settings.hpp>
#include "service-bluetooth/messages/AudioVolume.hpp"
#include "service-bluetooth/messages/AudioRouting.hpp"
#include "service-bluetooth/messages/Connect.hpp"
#include <service-bluetooth/messages/DeviceName.hpp>
#include "service-bluetooth/messages/Disconnect.hpp"
#include "service-bluetooth/messages/Status.hpp"
#include "service-bluetooth/messages/SetStatus.hpp"
#include "service-bluetooth/messages/BondedDevices.hpp"
#include "service-bluetooth/messages/Unpair.hpp"
#include "service-bluetooth/messages/SetDeviceName.hpp"
#include "service-bluetooth/BluetoothDevicesModel.hpp"
#include "service-bluetooth/messages/BluetoothModeChanged.hpp"
#include "service-bluetooth/messages/RequestStatusIndicatorData.hpp"

#include "system/messages/SentinelRegistrationMessage.hpp"

#include <log/log.hpp>
#include <bits/exception.h>
#include <utility>
#include <service-desktop/DesktopMessages.hpp>
#include <endpoints/bluetooth/BluetoothEventMessages.hpp>
#include <endpoints/bluetooth/BluetoothHelper.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <BtCommand.hpp>
#include <BtKeysStorage.hpp>
#include <Timers/TimerFactory.hpp>
#include <typeinfo>
#include <service-bluetooth/messages/Authenticate.hpp>
#include <GAP/GAP.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-evtmgr/BatteryMessages.hpp>

namespace
{
    constexpr auto BluetoothServiceStackDepth = 2560U;
    inline constexpr auto nameSettings        = "ApplicationSettings";
    inline constexpr auto connectionTimeout   = std::chrono::minutes{10};
    inline constexpr auto btRestartDelay      = std::chrono::milliseconds{500};

} // namespace

ServiceBluetooth::ServiceBluetooth() : sys::Service(service::name::bluetooth, "", BluetoothServiceStackDepth)
{
    LOG_INFO("[ServiceBluetooth] Initializing");
    bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);
}

ServiceBluetooth::~ServiceBluetooth()
{
    LOG_INFO("[ServiceBluetooth] Cleaning resources");
}

sys::ReturnCodes ServiceBluetooth::InitHandler()
{
    auto settings = std::make_unique<settings::Settings>();
    settings->init(service::ServiceProxy(shared_from_this()));
    settingsHolder                  = std::make_shared<bluetooth::SettingsHolder>(std::move(settings));
    bluetoothDevicesModel           = std::make_shared<BluetoothDevicesModel>(this);
    bluetooth::KeyStorage::settings = settingsHolder;

    bus.channels.push_back(sys::BusChannel::BluetoothNotifications);
    bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);

    worker = std::make_unique<BluetoothWorker>(this);
    worker->run();

    cpuSentinel = std::make_shared<sys::CpuSentinel>(service::name::bluetooth, this);

    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    bus.sendUnicast(sentinelRegistrationMsg, service::name::system_manager);

    connectionTimeoutTimer = sys::TimerFactory::createSingleShotTimer(
        this, "btTimeoutTimer", connectionTimeout, [this](sys::Timer &_) { handleTurnOff(); });
    startTimeoutTimer();

    connectHandler<BluetoothAddrMessage>();
    connectHandler<BluetoothAudioStartMessage>();
    connectHandler<BluetoothMessage>();
    connectHandler<BluetoothPairMessage>();
    connectHandler<BluetoothPairResultMessage>();
    connectHandler<message::bluetooth::A2DPVolume>();
    connectHandler<message::bluetooth::HSPVolume>();
    connectHandler<message::bluetooth::HFPVolume>();
    connectHandler<message::bluetooth::StartAudioRouting>();
    connectHandler<message::bluetooth::Connect>();
    connectHandler<message::bluetooth::ConnectResult>();
    connectHandler<message::bluetooth::Disconnect>();
    connectHandler<message::bluetooth::DisconnectResult>();
    connectHandler<message::bluetooth::RequestBondedDevices>();
    connectHandler<message::bluetooth::RequestDeviceName>();
    connectHandler<message::bluetooth::RequestStatus>();
    connectHandler<message::bluetooth::SetDeviceName>();
    connectHandler<message::bluetooth::SetStatus>();
    connectHandler<message::bluetooth::Unpair>();
    connectHandler<sdesktop::developerMode::DeveloperModeRequest>();
    connectHandler<message::bluetooth::ResponseAuthenticatePin>();
    connectHandler<message::bluetooth::ResponseAuthenticatePasskey>();
    connectHandler<message::bluetooth::ResponseAuthenticatePairCancel>();
    connectHandler<message::bluetooth::RequestStatusIndicatorData>();
    connectHandler<CellularCallerIdMessage>();
    connectHandler<CellularCallActiveNotification>();
    connectHandler<CellularIncominCallMessage>();
    connectHandler<cellular::CallEndedNotification>();
    connectHandler<cellular::CallStartedNotification>();
    connectHandler<CellularSignalStrengthUpdateNotification>();
    connectHandler<CellularCurrentOperatorNameNotification>();
    connectHandler<CellularNetworkStatusUpdateNotification>();
    connectHandler<sevm::BatteryStatusChangeMessage>();

    settingsHolder->onStateChange = [this]() {
        auto initialState = std::visit(bluetooth::IntVisitor(), settingsHolder->getValue(bluetooth::Settings::State));
        if (static_cast<BluetoothStatus::State>(initialState) == BluetoothStatus::State::On) {
            settingsHolder->setValue(bluetooth::Settings::State, static_cast<int>(BluetoothStatus::State::Off));
        }
    };

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceBluetooth::DeinitHandler()
{
    settingsHolder->deinit();
    worker->closeWorker();
    worker.reset();
    return sys::ReturnCodes::Success;
}

void ServiceBluetooth::ProcessCloseReason(sys::CloseReason closeReason)
{
    sendWorkerCommand(std::make_unique<bt::evt::DisconnectAudio>());
    sendWorkerCommand(std::make_unique<bt::evt::PowerOff>());
}

sys::MessagePointer ServiceBluetooth::DataReceivedHandler([[maybe_unused]] sys::DataMessage *msg,
                                                          [[maybe_unused]] sys::ResponseMessage *resp)
{
    return std::make_shared<sys::ResponseMessage>();
}

sys::ReturnCodes ServiceBluetooth::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_ERROR("TODO");
    return sys::ReturnCodes::Success;
}

auto ServiceBluetooth::handle(BluetoothAudioStartMessage *msg) -> std::shared_ptr<sys::Message>
{
    worker->setAudioDevice(msg->getAudioDevice());
    return std::make_shared<sys::ResponseMessage>();
}

auto ServiceBluetooth::handle([[maybe_unused]] message::bluetooth::RequestBondedDevices *msg)
    -> std::shared_ptr<sys::Message>
{
    auto bondedDevicesStr =
        std::visit(bluetooth::StringVisitor(), this->settingsHolder->getValue(bluetooth::Settings::BondedDevices));
    bluetoothDevicesModel->mergeDevicesList(SettingsSerializer::fromString(bondedDevicesStr));

    bus.sendMulticast(
        std::make_shared<message::bluetooth::ResponseBondedDevices>(bluetoothDevicesModel->getDevices(), ""),
        sys::BusChannel::BluetoothNotifications);
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle([[maybe_unused]] message::bluetooth::RequestStatus *msg) -> std::shared_ptr<sys::Message>
{
    auto state      = std::visit(bluetooth::IntVisitor(), settingsHolder->getValue(bluetooth::Settings::State));
    auto visibility = std::visit(bluetooth::BoolVisitor(), settingsHolder->getValue(bluetooth::Settings::Visibility));

    BluetoothStatus status{static_cast<BluetoothStatus::State>(state), status.visibility = visibility};
    bus.sendMulticast(std::make_shared<message::bluetooth::ResponseStatus>(status),
                      sys::BusChannel::BluetoothNotifications);
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::SetStatus *msg) -> std::shared_ptr<sys::Message>
{
    auto newBtStatus = msg->getStatus();

    switch (newBtStatus.state) {
    case BluetoothStatus::State::On:

        cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_3);
        sendWorkerCommand(std::make_unique<bt::evt::PowerOn>());
        bus.sendMulticast(
            std::make_shared<sys::bluetooth::BluetoothModeChanged>(sys::bluetooth::BluetoothMode::Enabled),
            sys::BusChannel::BluetoothModeChanges);
        {
            auto bondedDevicesStr = std::visit(bluetooth::StringVisitor(),
                                               this->settingsHolder->getValue(bluetooth::Settings::BondedDevices));
            bluetoothDevicesModel->mergeDevicesList(SettingsSerializer::fromString(bondedDevicesStr));
            bluetoothDevicesModel->syncDevicesWithApp();
        }
        startTimeoutTimer();
        break;
    case BluetoothStatus::State::Off:
        stopTimeoutTimer();
        handleTurnOff();
        break;
    default:
        break;
    }
    if (newBtStatus.visibility) {
        sendWorkerCommand(std::make_unique<bt::evt::VisibilityOn>());
    }
    else {
        sendWorkerCommand(std::make_unique<bt::evt::VisibilityOff>());
    }
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(BluetoothPairMessage *msg) -> std::shared_ptr<sys::Message>
{
    auto device = msg->getDevice();
    bluetoothDevicesModel->removeDevice(device);

    sendWorkerCommand(std::make_unique<bt::evt::Pair>(device));

    device.deviceState = DeviceState::Pairing;
    bluetoothDevicesModel->insertDevice(device);

    bluetoothDevicesModel->syncDevicesWithApp();

    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(BluetoothPairResultMessage *msg) -> std::shared_ptr<sys::Message>
{
    auto device = msg->getDevice();
    if (msg->isSucceed()) {
        bluetoothDevicesModel->mergeDevicesList(device);
        bluetoothDevicesModel->setInternalDeviceState(device, DeviceState::Paired);
    }
    else {
        bluetoothDevicesModel->removeDevice(device);
    }
    bluetoothDevicesModel->syncDevicesWithApp();

    /// TODO error code handing added in next PRs
    bus.sendMulticast(std::make_shared<BluetoothPairResultMessage>(msg->getDevice(), msg->isSucceed()),
                      sys::BusChannel::BluetoothNotifications);

    return sys::MessageNone{};
}

void ServiceBluetooth::sendWorkerCommand(std::unique_ptr<bt::evt::Base> command)
{
    if (workerQueue != nullptr) {
        workerQueue->push(bluetooth::Command{std::move(command)});
    }
}

auto ServiceBluetooth::handle(message::bluetooth::Unpair *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(std::make_unique<bt::evt::Unpair>(msg->getDevice()));
    bluetoothDevicesModel->removeDevice(msg->getDevice());
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle([[maybe_unused]] message::bluetooth::RequestDeviceName *msg)
    -> std::shared_ptr<sys::Message>
{
    auto deviceNameString =
        std::visit(bluetooth::StringVisitor(), this->settingsHolder->getValue(bluetooth::Settings::DeviceName));

    bus.sendMulticast(std::make_shared<message::bluetooth::ResponseDeviceName>(std::move(deviceNameString)),
                      sys::BusChannel::BluetoothNotifications);
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::SetDeviceName *msg) -> std::shared_ptr<sys::Message>
{
    auto newName = msg->getName();
    bluetooth::set_name(newName);
    settingsHolder->setValue(bluetooth::Settings::DeviceName, newName);
    sendWorkerCommand(std::make_unique<bt::evt::PowerOff>());

    btRestartTimer =
        sys::TimerFactory::createSingleShotTimer(this, "btRestartTimer", btRestartDelay, [this](sys::Timer &_) {
            sendWorkerCommand(std::make_unique<bt::evt::PowerOn>());
        });
    btRestartTimer.start();

    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::Connect *msg) -> std::shared_ptr<sys::Message>
{
    auto device = msg->getDevice();
    sendWorkerCommand(std::make_unique<bt::evt::ConnectAudio>(device));
    bluetoothDevicesModel->setInternalDeviceState(device, DeviceState::Connecting);
    bluetoothDevicesModel->syncDevicesWithApp();
    return sys::MessageNone{};
}

auto convertDeviceStateIntoBluetoothState(const DeviceState &state) -> sys::bluetooth::BluetoothMode
{
    switch (state) {
    case DeviceState::ConnectedVoice:
        return sys::bluetooth::BluetoothMode::ConnectedVoice;
    case DeviceState::ConnectedAudio:
        return sys::bluetooth::BluetoothMode::ConnectedAudio;
    case DeviceState::ConnectedBoth:
        return sys::bluetooth::BluetoothMode::ConnectedBoth;
    default:
        return sys::bluetooth::BluetoothMode::Enabled;
    }
}

auto ServiceBluetooth::handle(message::bluetooth::ConnectResult *msg) -> std::shared_ptr<sys::Message>
{
    if (msg->isSucceed()) {
        auto device = msg->getDevice();
        bluetoothDevicesModel->mergeInternalDeviceState(device);

        settingsHolder->setValue(bluetooth::Settings::ConnectedDevice, bd_addr_to_str(device.address));

        auto deviceState = bluetoothDevicesModel->getDeviceByAddress(device.address)->get().deviceState;
        bus.sendMulticast(
            std::make_shared<sys::bluetooth::BluetoothModeChanged>(convertDeviceStateIntoBluetoothState(deviceState)),
            sys::BusChannel::BluetoothModeChanges);

        stopTimeoutTimer();
    }

    for (auto &device : bluetoothDevicesModel->getDevices()) {
        if (device.deviceState == DeviceState::Connecting) {
            device.deviceState = DeviceState::Paired;
        }
    }

    bluetoothDevicesModel->syncDevicesWithApp();
    bus.sendMulticast(std::make_shared<message::bluetooth::ConnectResult>(*msg),
                      sys::BusChannel::BluetoothNotifications);
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle([[maybe_unused]] message::bluetooth::Disconnect *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(std::make_unique<bt::evt::DisconnectAudio>());
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::DisconnectResult *msg) -> std::shared_ptr<sys::Message>
{
    auto deviceAddr =
        std::visit(bluetooth::StringVisitor(), this->settingsHolder->getValue(bluetooth::Settings::ConnectedDevice));

    auto device = bluetoothDevicesModel->getDeviceByAddress(deviceAddr);
    if (device.has_value()) {
        device.value().get().deviceState = DeviceState::Paired;
    }
    bluetoothDevicesModel->syncDevicesWithApp();
    settingsHolder->setValue(bluetooth::Settings::ConnectedDevice, std::string());

    if (auto btOn = std::visit(bluetooth::BoolVisitor(), settingsHolder->getValue(bluetooth::Settings::State)); btOn) {
        bus.sendMulticast(
            std::make_shared<sys::bluetooth::BluetoothModeChanged>(sys::bluetooth::BluetoothMode::Enabled),
            sys::BusChannel::BluetoothModeChanges);

        startTimeoutTimer();
    }

    bus.sendMulticast(std::make_shared<message::bluetooth::DisconnectResult>(msg->getDevice()),
                      sys::BusChannel::BluetoothNotifications);

    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::ResponseAuthenticatePin *msg) -> std::shared_ptr<sys::Message>
{
    /// TODO to be added in next PRs
    auto pin = msg->getPin();
    bluetooth::GAP::respondPinCode(pin, msg->getDevice());
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::ResponseAuthenticatePasskey *msg) -> std::shared_ptr<sys::Message>
{
    auto passKey = msg->getPasskey();
    bluetooth::GAP::respondPinCode(passKey, msg->getDevice());
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::ResponseAuthenticatePairCancel *msg) -> std::shared_ptr<sys::Message>
{
    auto accepted = msg->getPairApproved();
    bluetooth::GAP::finishCodeComparison(accepted, msg->getDevice());
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(BluetoothMessage *msg) -> std::shared_ptr<sys::Message>
{
    LOG_INFO("Bluetooth request!");
    resetTimeoutTimer();

    switch (msg->req) {
    case BluetoothMessage::Scan:
        sendWorkerCommand(std::make_unique<bt::evt::StartScan>());
        break;
    case BluetoothMessage::StopScan:
        sendWorkerCommand(std::make_unique<bt::evt::StopScan>());
        break;
    case BluetoothMessage::getDevicesAvailable:
        sendWorkerCommand(std::make_unique<bt::evt::GetDevicesAvailable>());
        break;
    case BluetoothMessage::Visible: {
        auto visibility =
            not std::visit(bluetooth::BoolVisitor(), settingsHolder->getValue(bluetooth::Settings::Visibility));
        if (visibility) {
            sendWorkerCommand(std::make_unique<bt::evt::VisibilityOn>());
        }
        else {
            sendWorkerCommand(std::make_unique<bt::evt::VisibilityOff>());
        }
    } break;
    case BluetoothMessage::Play:
        sendWorkerCommand(std::make_unique<bt::evt::StartStream>());
        break;
    case BluetoothMessage::Disconnect:
        sendWorkerCommand(std::make_unique<bt::evt::DisconnectAudio>());
        break;
    case BluetoothMessage::Stop:
        sendWorkerCommand(std::make_unique<bt::evt::StopStream>());
        break;
    default:
        break;
    }

    return std::make_shared<sys::ResponseMessage>();
}

auto ServiceBluetooth::handle(BluetoothAddrMessage *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(std::make_unique<bt::evt::ConnectAudio>(msg->device));
    return std::make_shared<sys::ResponseMessage>();
}

auto ServiceBluetooth::handle(sdesktop::developerMode::DeveloperModeRequest *msg) -> std::shared_ptr<sys::Message>
{
    if (typeid(*msg->event) == typeid(sdesktop::bluetooth::GetAvailableDevicesEvent)) {
        sendWorkerCommand(std::make_unique<bt::evt::GetDevicesAvailable>());
    }
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::A2DPVolume *msg) -> std::shared_ptr<sys::Message>
{
    using namespace message::bluetooth;
    AudioServiceAPI::BluetoothA2DPVolumeChanged(this, msg->getVolume());
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::HSPVolume *msg) -> std::shared_ptr<sys::Message>
{
    using namespace message::bluetooth;
    AudioServiceAPI::BluetoothHSPVolumeChanged(this, msg->getVolume());
    return sys::MessageNone{};
}
auto ServiceBluetooth::handle(message::bluetooth::HFPVolume *msg) -> std::shared_ptr<sys::Message>
{
    using namespace message::bluetooth;
    AudioServiceAPI::BluetoothHFPVolumeChanged(this, msg->getVolume());
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::StartAudioRouting *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(std::make_unique<bt::evt::StartRouting>());
    return std::make_shared<sys::ResponseMessage>();
}

auto ServiceBluetooth::handle(CellularCallerIdMessage *msg) -> std::shared_ptr<sys::Message>
{
    auto number = msg->number;
    auto btOn   = std::visit(bluetooth::BoolVisitor(), settingsHolder->getValue(bluetooth::Settings::State));
    LOG_DEBUG("Received caller ID msg! ");

    if (btOn) {
        LOG_DEBUG("Sending to profile!");
        sendWorkerCommand(std::make_unique<bt::evt::IncomingCallNumber>(number));
    }

    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(CellularCallActiveNotification *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(std::make_unique<bt::evt::CallAnswered>());
    return std::make_shared<sys::ResponseMessage>();
}

auto ServiceBluetooth::handle(CellularSignalStrengthUpdateNotification *msg) -> std::shared_ptr<sys::Message>
{
    auto signalStrength = Store::GSM::get()->getSignalStrength();
    LOG_DEBUG("Bluetooth: RSSI %d/5", static_cast<int>(signalStrength.rssiBar));
    sendWorkerCommand(std::make_unique<bt::evt::SignalStrengthData>(signalStrength));
    return std::make_shared<sys::ResponseMessage>();
}

auto ServiceBluetooth::handle(CellularCurrentOperatorNameNotification *msg) -> std::shared_ptr<sys::Message>
{
    auto opName = msg->getCurrentOperatorName();
    LOG_DEBUG("Bluetooth: Operator name: %s", opName.c_str());
    sendWorkerCommand(std::make_unique<bt::evt::OperatorNameData>(opName));
    return std::make_shared<sys::ResponseMessage>();
}

void ServiceBluetooth::startTimeoutTimer()
{
    if (connectionTimeoutTimer.isValid()) {
        connectionTimeoutTimer.start();
    }
}

void ServiceBluetooth::stopTimeoutTimer()
{
    if (connectionTimeoutTimer.isValid()) {
        connectionTimeoutTimer.stop();
    }
}

void ServiceBluetooth::resetTimeoutTimer()
{
    if (connectionTimeoutTimer.isValid() && connectionTimeoutTimer.isActive()) {
        connectionTimeoutTimer.stop();
        connectionTimeoutTimer.start();
    }
}

void ServiceBluetooth::handleTurnOff()
{
    sendWorkerCommand(std::make_unique<bt::evt::PowerOff>());
    // NOTE: This should be in bluetooth state machine
    cpuSentinel->ReleaseMinimumFrequency();
    bus.sendMulticast(std::make_shared<sys::bluetooth::BluetoothModeChanged>(sys::bluetooth::BluetoothMode::Disabled),
                      sys::BusChannel::BluetoothModeChanges);
}
auto ServiceBluetooth::handle(message::bluetooth::RequestStatusIndicatorData *msg) -> std::shared_ptr<sys::Message>
{
    bus.sendUnicast(std::make_shared<CellularRequestCurrentOperatorNameMessage>(), cellular::service::name);

    // just to execute proper handle method and sending it back to worker
    bus.sendUnicast(std::make_shared<CellularSignalStrengthUpdateNotification>(), service::name::bluetooth);
    bus.sendUnicast(std::make_shared<sevm::BatteryStatusChangeMessage>(), service::name::bluetooth);
    bus.sendUnicast(std::make_shared<CellularNetworkStatusUpdateNotification>(), service::name::bluetooth);

    return sys::MessageNone{};
}
auto ServiceBluetooth::handle(sevm::BatteryStatusChangeMessage *msg) -> std::shared_ptr<sys::Message>
{
    auto batteryLevel = Store::Battery::get().level;
    LOG_DEBUG("Bluetooth: Battery level %d", batteryLevel);
    sendWorkerCommand(std::make_unique<bt::evt::BatteryLevelData>(batteryLevel));
    return sys::MessageNone{};
}
auto ServiceBluetooth::handle(cellular::CallEndedNotification *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(std::make_unique<bt::evt::CallTerminated>());
    return sys::MessageNone{};
}
auto ServiceBluetooth::handle(CellularNetworkStatusUpdateNotification *msg) -> std::shared_ptr<sys::Message>
{
    auto status = Store::GSM::get()->getNetwork().status;
    LOG_DEBUG("Bluetooth: Network status %s", magic_enum::enum_name(status).data());
    sendWorkerCommand(std::make_unique<bt::evt::NetworkStatusData>(status));
    return sys::MessageNone{};
}
auto ServiceBluetooth::handle(cellular::CallStartedNotification *msg) -> std::shared_ptr<sys::Message>
{
    if (!msg->isCallIncoming()) {
        auto evt = std::make_unique<bt::evt::CallStarted>(msg->getNumber());
        sendWorkerCommand(std::move(evt));
    }
    return sys::MessageNone{};
}
auto ServiceBluetooth::handle(CellularIncominCallMessage *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(std::make_unique<bt::evt::StartRinging>());
    return sys::MessageNone{};
}
