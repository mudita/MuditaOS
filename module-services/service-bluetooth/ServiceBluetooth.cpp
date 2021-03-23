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
#include "service-bluetooth/messages/Connect.hpp"
#include "service-bluetooth/messages/Disconnect.hpp"
#include "service-bluetooth/messages/Status.hpp"
#include "service-bluetooth/messages/SetStatus.hpp"
#include "service-bluetooth/messages/BondedDevices.hpp"
#include "service-bluetooth/messages/Unpair.hpp"
#include "service-bluetooth/messages/SetDeviceName.hpp"

#include "SystemManager/messages/SentinelRegistrationMessage.hpp"

#include <log/log.hpp>
#include <bits/exception.h>
#include <utility>
#include <service-desktop/service-desktop/DesktopMessages.hpp>
#include <service-desktop/endpoints/bluetooth/BluetoothEventMessages.hpp>
#include <service-desktop/endpoints/bluetooth/BluetoothHelper.hpp>
#include <BtCommand.hpp>
#include <BtKeysStorage.hpp>

#include <typeinfo>

namespace
{
    constexpr auto BluetoothServiceStackDepth = 2560U;
    inline constexpr auto nameSettingsNew     = "ApplicationSettingsNew";
} // namespace

ServiceBluetooth::ServiceBluetooth() : sys::Service(service::name::bluetooth, "", BluetoothServiceStackDepth)
{
    auto settings                   = std::make_unique<settings::Settings>(this);
    settingsHolder                  = std::make_shared<bluetooth::SettingsHolder>(std::move(settings));
    bluetooth::KeyStorage::settings = settingsHolder;
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
    worker = std::make_unique<BluetoothWorker>(this);
    worker->run();

    cpuSentinel = std::make_shared<sys::CpuSentinel>(service::name::bluetooth, this);

    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    bus.sendUnicast(sentinelRegistrationMsg, service::name::system_manager);

    connect(message::bluetooth::RequestBondedDevices(), [&](sys::Message *msg) {
        auto request = static_cast<message::bluetooth::RequestBondedDevices *>(msg);
        return handle(request);
    });

    connect(message::bluetooth::RequestStatus(), [&](sys::Message *msg) {
        auto request = static_cast<message::bluetooth::RequestStatus *>(msg);
        return handle(request);
    });

    connect(typeid(message::bluetooth::SetStatus), [&](sys::Message *msg) {
        auto request = static_cast<message::bluetooth::SetStatus *>(msg);
        return handle(request);
    });

    connect(typeid(BluetoothPairMessage), [&](sys::Message *msg) {
        auto request = static_cast<BluetoothPairMessage *>(msg);
        return handle(request);
    });
    connect(typeid(message::bluetooth::Unpair), [&](sys::Message *msg) {
        auto request = static_cast<message::bluetooth::Unpair *>(msg);
        return handle(request);
    });

    connect(typeid(message::bluetooth::SetDeviceName), [&](sys::Message *msg) {
        auto request = static_cast<message::bluetooth::SetDeviceName *>(msg);
        return handle(request);
    });

    connect(typeid(sdesktop::developerMode::DeveloperModeRequest), [&](sys::Message *msg) {
        auto request = static_cast<sdesktop::developerMode::DeveloperModeRequest *>(msg);
        return handle(request);
    });

    connect(typeid(message::bluetooth::Connect), [&](sys::Message *msg) {
        auto request = static_cast<message::bluetooth::Connect *>(msg);
        return handle(request);
    });

    connect(typeid(message::bluetooth::Disconnect), [&](sys::Message *msg) {
        auto request = static_cast<message::bluetooth::Disconnect *>(msg);
        return handle(request);
    });

    connect(typeid(BluetoothMessage), [&](sys::Message *msg) {
        auto request = static_cast<BluetoothMessage *>(msg);
        return handle(request);
    });

    connect(typeid(BluetoothAddrMessage), [&](sys::Message *msg) {
        auto request = static_cast<BluetoothAddrMessage *>(msg);
        return handle(request);
    });

    connect(typeid(message::bluetooth::ConnectResult), [&](sys::Message *msg) {
        auto result = static_cast<message::bluetooth::ConnectResult *>(msg);
        return handle(result);
    });

    connect(typeid(message::bluetooth::DisconnectResult), [&](sys::Message *msg) {
        auto result = static_cast<message::bluetooth::DisconnectResult *>(msg);
        return handle(result);
    });

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
        break;
    case BluetoothStatus::State::Off:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::PowerOff));
        cpuSentinel->ReleaseMinimumFrequency();
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

auto ServiceBluetooth::handle(message::bluetooth::SetDeviceName *msg) -> std::shared_ptr<sys::Message>
{
    auto newName = msg->getName();
    bluetooth::set_name(newName);
    settingsHolder->setValue(bluetooth::Settings::DeviceName, newName);
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
    }
    bus.sendUnicast(std::make_shared<message::bluetooth::ConnectResult>(*msg), nameSettingsNew);
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
    auto bondedDevicesStr =
        std::visit(bluetooth::StringVisitor(), this->settingsHolder->getValue(bluetooth::Settings::BondedDevices));
    bus.sendUnicast(std::make_shared<message::bluetooth::ResponseBondedDevices>(
                        SettingsSerializer::fromString(bondedDevicesStr), std::string()),
                    nameSettingsNew);
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
        break;
    case BluetoothMessage::SwitchProfile:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::SwitchProfile));
        break;
    case BluetoothMessage::Disconnect:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::DisconnectAudio));
        break;
    case BluetoothMessage::Stop:
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::StopStream));
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
