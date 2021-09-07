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
#include "service-bluetooth/messages/BluetoothModeChanged.hpp"

#include "SystemManager/messages/SentinelRegistrationMessage.hpp"

#include <log.hpp>
#include <bits/exception.h>
#include <utility>
#include <service-desktop/service-desktop/DesktopMessages.hpp>
#include <service-desktop/endpoints/bluetooth/BluetoothEventMessages.hpp>
#include <service-desktop/endpoints/bluetooth/BluetoothHelper.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <BtCommand.hpp>
#include <BtKeysStorage.hpp>
#include <Timers/TimerFactory.hpp>
#include <typeinfo>
#include <service-bluetooth/messages/Passkey.hpp>
#include <GAP/GAP.hpp>

namespace
{
    constexpr auto BluetoothServiceStackDepth = 2560U;
    inline constexpr auto nameSettings        = "ApplicationSettings";
    inline constexpr auto connectionTimeout   = std::chrono::minutes{30};
} // namespace

ServiceBluetooth::ServiceBluetooth() : sys::Service(service::name::bluetooth, "", BluetoothServiceStackDepth)
{
    LOG_INFO("[ServiceBluetooth] Initializing");
}

ServiceBluetooth::~ServiceBluetooth()
{
    LOG_INFO("[ServiceBluetooth] Cleaning resources");
}

// This code is experimental:
// this means it is an init point of bluetooth feature handling
sys::ReturnCodes ServiceBluetooth::InitHandler()
{
    auto settings = std::make_unique<settings::Settings>();
    settings->init(service::ServiceProxy(shared_from_this()));
    settingsHolder                  = std::make_shared<bluetooth::SettingsHolder>(std::move(settings));
    bluetooth::KeyStorage::settings = settingsHolder;

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
    LOG_INFO("Requested bonded devices!");
    auto bondedDevicesStr =
        std::visit(bluetooth::StringVisitor(), this->settingsHolder->getValue(bluetooth::Settings::BondedDevices));
    auto connectedAddress =
        std::visit(bluetooth::StringVisitor(), this->settingsHolder->getValue(bluetooth::Settings::ConnectedDevice));

    return std::make_shared<message::bluetooth::ResponseBondedDevices>(SettingsSerializer::fromString(bondedDevicesStr),
                                                                       std::move(connectedAddress));
}

auto ServiceBluetooth::handle([[maybe_unused]] message::bluetooth::RequestStatus *msg) -> std::shared_ptr<sys::Message>
{
    auto state      = std::visit(bluetooth::IntVisitor(), settingsHolder->getValue(bluetooth::Settings::State));
    auto visibility = std::visit(bluetooth::BoolVisitor(), settingsHolder->getValue(bluetooth::Settings::Visibility));

    BluetoothStatus status{static_cast<BluetoothStatus::State>(state), status.visibility = visibility};
    return std::make_shared<message::bluetooth::ResponseStatus>(status);
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
    const auto addrString = msg->addr;
    bd_addr_t addr;
    sscanf_bd_addr(addrString.c_str(), addr);
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::Pair, addr));
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::Unpair *msg) -> std::shared_ptr<sys::Message>
{
    const auto addrString = msg->getAddr();
    bd_addr_t addr;
    sscanf_bd_addr(addrString.c_str(), addr);
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::Unpair, addr));
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle([[maybe_unused]] message::bluetooth::RequestDeviceName *msg)
    -> std::shared_ptr<sys::Message>
{
    auto deviceNameString =
        std::visit(bluetooth::StringVisitor(), this->settingsHolder->getValue(bluetooth::Settings::DeviceName));

    return std::make_shared<message::bluetooth::ResponseDeviceName>(std::move(deviceNameString));
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
    const auto addrString = msg->getAddr();
    LOG_DEBUG("Connecting with %s", addrString.c_str());
    bd_addr_t addr;
    sscanf_bd_addr(addrString.c_str(), addr);
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::ConnectAudio, addr));
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::ConnectResult *msg) -> std::shared_ptr<sys::Message>
{
    if (msg->isSucceed()) {
        settingsHolder->setValue(bluetooth::Settings::ConnectedDevice, msg->getAddr());
        startTimeoutTimer();
        bus.sendMulticast(
            std::make_shared<sys::bluetooth::BluetoothModeChanged>(sys::bluetooth::BluetoothMode::Connected),
            sys::BusChannel::BluetoothModeChanges);
    }
    bus.sendUnicast(std::make_shared<message::bluetooth::ConnectResult>(*msg), nameSettings);
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle([[maybe_unused]] message::bluetooth::Disconnect *msg) -> std::shared_ptr<sys::Message>
{
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::DisconnectAudio));
    return sys::MessageNone{};
}

auto ServiceBluetooth::handle(message::bluetooth::DisconnectResult *msg) -> std::shared_ptr<sys::Message>
{
    settingsHolder->setValue(bluetooth::Settings::ConnectedDevice, std::string());
    bus.sendMulticast(std::make_shared<sys::bluetooth::BluetoothModeChanged>(sys::bluetooth::BluetoothMode::Enabled),
                      sys::BusChannel::BluetoothModeChanges);
    auto bondedDevicesStr =
        std::visit(bluetooth::StringVisitor(), this->settingsHolder->getValue(bluetooth::Settings::BondedDevices));
    bus.sendUnicast(std::make_shared<message::bluetooth::ResponseBondedDevices>(
                        SettingsSerializer::fromString(bondedDevicesStr), std::string()),
                    nameSettings);
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
    case BluetoothMessage::PAN: {
        /// TODO request lwip first...
        /// because TODO blocking message - wrecks system
        LOG_INFO("Request LwIP running!");
        //                    auto ret = message_lwip(this, LwIP_message::Request::Start);
        //                    if (ret != sys::ReturnCodes::Success) {
        //                        LOG_ERROR("Request for LwIP start failed");
        //                    }
        //                    else {
        /// TODO request PPP
        LOG_INFO("Start PAN");
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::StartPan));
        //                    }
    } break;
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
    std::string addrString{bd_addr_to_str(msg->addr)};
    LOG_INFO("Connecting with %s", addrString.c_str());
    sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::ConnectAudio, msg->addr));
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
    AudioServiceAPI::BluetoothHSPVolumeChanged(this, msg->getVolume());
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
