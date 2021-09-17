// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
#include "service-bluetooth/messages/Ring.hpp"
#include "service-bluetooth/BluetoothDevicesModel.hpp"
#include "service-bluetooth/messages/BluetoothModeChanged.hpp"

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
#include <service-bluetooth/messages/Passkey.hpp>
#include <GAP/GAP.hpp>
#include <service-cellular/CellularMessage.hpp>

namespace
{
    constexpr auto BluetoothServiceStackDepth = 2560U;
    inline constexpr auto nameSettings        = "ApplicationSettings";
    inline constexpr auto connectionTimeout   = std::chrono::minutes{30};
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

    worker = std::make_unique<BluetoothWorker>(this);
    worker->run();

    cpuSentinel = std::make_shared<sys::CpuSentinel>(service::name::bluetooth, this);

    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    bus.sendUnicast(sentinelRegistrationMsg, service::name::system_manager);

    connectionTimeoutTimer =
        sys::TimerFactory::createSingleShotTimer(this, "btTimeoutTimer", connectionTimeout, [this](sys::Timer &_) {
            sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::DisconnectAudio));
            LOG_ERROR("Disconnecting from timeout timer!");
        });

    connectHandler<BluetoothAddrMessage>();
    connectHandler<BluetoothAudioStartMessage>();
    connectHandler<BluetoothMessage>();
    connectHandler<BluetoothPairMessage>();
    connectHandler<BluetoothPairResultMessage>();
    connectHandler<message::bluetooth::A2DPVolume>();
    connectHandler<message::bluetooth::HSPVolume>();
    connectHandler<message::bluetooth::Ring>();
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
    connectHandler<message::bluetooth::ResponsePasskey>();
    connectHandler<CellularCallerIdMessage>();
    connectHandler<CellularCallActiveNotification>();

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
    worker->deinit();
    return sys::ReturnCodes::Success;
}

void ServiceBluetooth::ProcessCloseReason(sys::CloseReason closeReason)
{
    sendCloseReadyMessage(this);
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
void ServiceBluetooth::sendWorkerCommand(bluetooth::Command command)
{
    xQueueSend(workerQueue, &command, portMAX_DELAY);
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

        cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyHz::Level_3);
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::PowerOn));
        bus.sendMulticast(
            std::make_shared<sys::bluetooth::BluetoothModeChanged>(sys::bluetooth::BluetoothMode::Enabled),
            sys::BusChannel::BluetoothModeChanges);
        {
            auto bondedDevicesStr = std::visit(bluetooth::StringVisitor(),
                                               this->settingsHolder->getValue(bluetooth::Settings::BondedDevices));
            bluetoothDevicesModel->mergeDevicesList(SettingsSerializer::fromString(bondedDevicesStr));
            bluetoothDevicesModel->syncDevicesWithApp();
        }
        break;
    case BluetoothStatus::State::Off:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::PowerOff));
        cpuSentinel->ReleaseMinimumFrequency();
        bus.sendMulticast(
            std::make_shared<sys::bluetooth::BluetoothModeChanged>(sys::bluetooth::BluetoothMode::Disabled),
            sys::BusChannel::BluetoothModeChanges);
        break;
    default:
        break;
    }
    bluetooth::Command command(newBtStatus.visibility ? bluetooth::Command::Type::VisibilityOn
                                                      : bluetooth::Command::Type::VisibilityOff);
    sendWorkerCommand(command);
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(BluetoothPairMessage *msg) -> std::shared_ptr<sys::Message>
{
    auto device = msg->getDevice();
    bluetoothDevicesModel->removeDevice(device);
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::Pair, device));

    device.deviceState = DeviceState::Pairing;
    bluetoothDevicesModel->insertDevice(device);

    bluetoothDevicesModel->syncDevicesWithApp();

    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(BluetoothPairResultMessage *msg) -> std::shared_ptr<sys::Message>
{
    auto device = msg->getDevice();
    if (msg->isSucceed()) {
        bluetoothDevicesModel->setInternalDeviceState(device, DeviceState::Paired);
    }
    else {
        bluetoothDevicesModel->removeDevice(device);
    }

    bluetoothDevicesModel->syncDevicesWithApp();

    bus.sendMulticast(std::make_shared<BluetoothPairResultMessage>(msg->getDevice(), msg->isSucceed()),
                      sys::BusChannel::BluetoothNotifications);

    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::Unpair *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::Unpair, msg->getDevice()));
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
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::PowerOff));
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::PowerOn));
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::Connect *msg) -> std::shared_ptr<sys::Message>
{
    auto device = msg->getDevice();
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::ConnectAudio, device));

    bluetoothDevicesModel->setInternalDeviceState(device, DeviceState::Connecting);
    bluetoothDevicesModel->syncDevicesWithApp();
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::ConnectResult *msg) -> std::shared_ptr<sys::Message>
{
    if (msg->isSucceed()) {
        auto device = msg->getDevice();
        bluetoothDevicesModel->setInternalDeviceState(device, DeviceState::Connected);

        settingsHolder->setValue(bluetooth::Settings::ConnectedDevice, bd_addr_to_str(device.address));
        startTimeoutTimer();
        bus.sendMulticast(
            std::make_shared<sys::bluetooth::BluetoothModeChanged>(sys::bluetooth::BluetoothMode::Connected),
            sys::BusChannel::BluetoothModeChanges);
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
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::DisconnectAudio));
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
    bus.sendMulticast(std::make_shared<sys::bluetooth::BluetoothModeChanged>(sys::bluetooth::BluetoothMode::Enabled),
                      sys::BusChannel::BluetoothModeChanges);

    bus.sendMulticast(std::make_shared<message::bluetooth::DisconnectResult>(msg->getDevice()),
                      sys::BusChannel::BluetoothNotifications);
    stopTimeoutTimer();

    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::ResponsePasskey *msg) -> std::shared_ptr<sys::Message>
{
    auto passKey = msg->getPasskey();
    bluetooth::GAP::respondPinCode(passKey);
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(BluetoothMessage *msg) -> std::shared_ptr<sys::Message>
{
    LOG_INFO("Bluetooth request!");
    switch (msg->req) {
    case BluetoothMessage::Start:
        break;
    case BluetoothMessage::Scan:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::StartScan));
        break;
    case BluetoothMessage::StopScan:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::StopScan));
        break;
    case BluetoothMessage::getDevicesAvailable:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::getDevicesAvailable));
        break;
    case BluetoothMessage::Visible: {
        auto visibility =
            not std::visit(bluetooth::BoolVisitor(), settingsHolder->getValue(bluetooth::Settings::Visibility));
        bluetooth::Command command(visibility ? bluetooth::Command::Type::VisibilityOn
                                              : bluetooth::Command::Type::VisibilityOff);
        sendWorkerCommand(command);
    } break;
    case BluetoothMessage::Play:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::StartStream));
        stopTimeoutTimer();

        break;
    case BluetoothMessage::SwitchProfile:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::SwitchProfile));
        break;
    case BluetoothMessage::Disconnect:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::DisconnectAudio));
        break;
    case BluetoothMessage::Stop:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::StopStream));
        startTimeoutTimer();
        break;
    default:
        break;
    }

    return std::make_shared<sys::ResponseMessage>();
}

auto ServiceBluetooth::handle(BluetoothAddrMessage *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::ConnectAudio, msg->device));
    return std::make_shared<sys::ResponseMessage>();
}

auto ServiceBluetooth::handle(sdesktop::developerMode::DeveloperModeRequest *msg) -> std::shared_ptr<sys::Message>
{
    if (typeid(*msg->event) == typeid(sdesktop::bluetooth::GetAvailableDevicesEvent)) {
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::getDevicesAvailable));
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
    AudioServiceAPI::BluetoothHSPV olumeChanged(this, msg->getVolume());
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::Ring *msg) -> std::shared_ptr<sys::Message>
{
    const auto enableRing = msg->enabled();
    sendWorkerCommand(bluetooth::Command(enableRing ? bluetooth::Command::Type::StartRinging
                                                    : bluetooth::Command::Type::StopRinging));

    return std::make_shared<sys::ResponseMessage>();
}

auto ServiceBluetooth::handle(message::bluetooth::StartAudioRouting *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::StartRouting));
    return std::make_shared<sys::ResponseMessage>();
}

auto ServiceBluetooth::handle(CellularCallerIdMessage *msg) -> std::shared_ptr<sys::Message>
{
    auto number = msg->number;
    auto btOn   = std::visit(bluetooth::BoolVisitor(), settingsHolder->getValue(bluetooth::Settings::State));
    LOG_ERROR("Received caller ID msg! Number: %s", number.getEntered().c_str());

    if (btOn) {
        LOG_ERROR("Sending to profile!");
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::IncomingCallNumber, number));
    }

    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(CellularCallActiveNotification *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::CallAnswered));
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
