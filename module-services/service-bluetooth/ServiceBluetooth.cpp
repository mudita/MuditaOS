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
#include "service-bluetooth/messages/Status.hpp"
#include "service-bluetooth/messages/SetStatus.hpp"
#include <service-bluetooth/messages/BondedDevices.hpp>

#include <log/log.hpp>
#include "SystemManager/messages/SentinelRegistrationMessage.hpp"

#include <bits/exception.h>
#include <utility>
#include <service-desktop/service-desktop/DesktopMessages.hpp>
#include <service-desktop/service-desktop/Constants.hpp>
#include <service-bluetooth/messages/SetDeviceName.hpp>
#include <BtCommand.hpp>
#include <BtKeysStorage.hpp>

ServiceBluetooth::ServiceBluetooth() : sys::Service(service::name::bluetooth, "", 4096)
{
    auto settings  = std::make_unique<settings::Settings>(this);
    settingsHolder = std::make_shared<bluetooth::SettingsHolder>(std::move(settings));
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
    cpuSentinel = std::make_shared<sys::CpuSentinel>(service::name::bluetooth, this);

    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);

    // temporarily limit the minimum CPU frequency
    // due to problems with the UART of the BT module
    cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyHz::Level_6);

    worker = std::make_unique<BluetoothWorker>(this);
    worker->run();

    connect(message::bluetooth::RequestBondedDevices(), [&](sys::Message *msg) {
        auto bondedDevicesStr =
            std::visit(bluetooth::StringVisitor(), this->settingsHolder->getValue(bluetooth::Settings::BondedDevices));

        return std::make_shared<message::bluetooth::ResponseBondedDevices>(
            SettingsSerializer::fromString(bondedDevicesStr));
    });

    connect(message::bluetooth::RequestStatus(), [&](sys::Message *msg) {
        BluetoothStatus status;

        auto state = std::visit(bluetooth::IntVisitor(), settingsHolder->getValue(bluetooth::Settings::State));
        auto visibility =
            std::visit(bluetooth::BoolVisitor(), settingsHolder->getValue(bluetooth::Settings::Visibility));
        status.state      = static_cast<BluetoothStatus::State>(state);
        status.visibility = visibility;

        return std::make_shared<message::bluetooth::ResponseStatus>(status);
    });

    connect(typeid(message::bluetooth::SetStatus), [&](sys::Message *msg) {
        auto setStatusMsg = static_cast<message::bluetooth::SetStatus *>(msg);
        auto newBtStatus  = setStatusMsg->getStatus();

        switch (newBtStatus.state) {
        case BluetoothStatus::State::On:
            if (msg->sender == "ServiceDesktop") {
                enabledFromHarness = true;
                LOG_INFO("BT enabled from Harness");
            }
            sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::PowerOn));
            break;
        case BluetoothStatus::State::Off:
            sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::PowerOff));
            enabledFromHarness = false;
            break;
        default:
            break;
        }
        bluetooth::Command command(newBtStatus.visibility ? bluetooth::Command::Type::VisibilityOn
                                                          : bluetooth::Command::Type::VisibilityOff);
        sendWorkerCommand(command);
        return sys::MessageNone{};
    });

    connect(typeid(BluetoothPairMessage), [&](sys::Message *msg) {
        auto pairMsg = static_cast<BluetoothPairMessage *>(msg);
        const auto addrString = pairMsg->addr;
        bd_addr_t addr;
        sscanf_bd_addr(addrString.c_str(), addr);
        sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::Pair, addr));
        return sys::MessageNone{};
    });

    connect(typeid(message::bluetooth::SetDeviceName), [&](sys::Message *msg) {
        auto setNameMsg = static_cast<message::bluetooth::SetDeviceName *>(msg);
        auto newName    = setNameMsg->getName();
        bluetooth::set_name(newName);
        settingsHolder->setValue(bluetooth::Settings::DeviceName, newName);
        return sys::MessageNone{};
    });

    connect(sdesktop::developerMode::DeveloperModeRequest(), [&](sys::Message *msg) {
        using namespace sdesktop::developerMode;
        auto req = static_cast<DeveloperModeRequest *>(msg);
        if (typeid(*req->event) == typeid(sdesktop::bluetooth::BluetoothStatusRequestEvent)) {
            auto state   = std::visit(bluetooth::IntVisitor(), settingsHolder->getValue(bluetooth::Settings::State));
            auto event   = std::make_unique<sdesktop::bluetooth::BluetoothStatusRequestEvent>(state);
            auto message = std::make_shared<DeveloperModeRequest>(std::move(event));
            bus.sendUnicast(std::move(message), service::name::service_desktop);
        }

        return sys::MessageNone{};
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

sys::MessagePointer ServiceBluetooth::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    try {
        switch (static_cast<MessageType>(msg->messageType)) {
        case MessageType::BluetoothRequest: {
            BluetoothMessage *lmsg = dynamic_cast<BluetoothMessage *>(msg);
            LOG_INFO("Bluetooth request!");
            switch (lmsg->req) {
            case BluetoothMessage::Start:
                break;
            case BluetoothMessage::Scan:
                sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::StartScan));
                break;
            case BluetoothMessage::StopScan:
                sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::StopScan));
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
                static bool visibility = true;
                bluetooth::Command command(visibility ? bluetooth::Command::Type::VisibilityOn
                                                      : bluetooth::Command::Type::VisibilityOff);
                sendWorkerCommand(command);
                visibility = !visibility;
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
            break;
        }
        case MessageType::BluetoothAddrResult: {
            auto addrMsg = static_cast<BluetoothAddrMessage *>(msg);
            std::string addrString{bd_addr_to_str(addrMsg->addr)};
            LOG_INFO("Connecting with %s", addrString.c_str());
            sendWorkerCommand(bluetooth::Command(bluetooth::Command::Type::ConnectAudio, addrMsg->addr));

        } break;
        default:
            break;
        }
    }
    catch (std::exception &ex) {
        LOG_ERROR("Exception on BtService!: %s", ex.what());
    }

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
void ServiceBluetooth::scanStartedCallback()
{
    if (enabledFromHarness) {
        auto event   = std::make_unique<sdesktop::bluetooth::ScanStartedEvent>();
        auto message = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        bus.sendUnicast(std::move(message), service::name::service_desktop);
    }
}
void ServiceBluetooth::scanStoppedCallback()
{
    if (enabledFromHarness) {
        auto event   = std::make_unique<sdesktop::bluetooth::ScanStoppedEvent>();
        auto message = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        bus.sendUnicast(std::move(message), service::name::service_desktop);
    }
}
