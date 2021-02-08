﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Constants.hpp"
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

#include <bits/exception.h>
#include <utility>
#include <service-desktop/service-desktop/DesktopMessages.hpp>
#include <service-desktop/service-desktop/Constants.hpp>

ServiceBluetooth::ServiceBluetooth() : sys::Service(service::name::bluetooth)
{
    auto settings  = std::make_unique<settings::Settings>(this);
    settingsHolder = std::make_shared<bluetooth::SettingsHolder>(std::move(settings));
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
    LOG_ERROR("Bluetooth experimental!");
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
            sendWorkerCommand(bluetooth::Command::PowerOn);
            break;
        case BluetoothStatus::State::Off:
            sendWorkerCommand(bluetooth::Command::PowerOff);
            break;
        default:
            break;
        }
        sendWorkerCommand(newBtStatus.visibility ? bluetooth::VisibilityOn : bluetooth::VisibilityOff);
        return sys::MessageNone{};
    });

    connect(sdesktop::developerMode::DeveloperModeRequest(), [&](sys::Message *msg) {
        using namespace sdesktop::developerMode;
        auto req = static_cast<DeveloperModeRequest *>(msg);
        if (typeid(*req->event) == typeid(BluetoothStatusRequestEvent)) {
            auto state   = std::visit(bluetooth::IntVisitor(), settingsHolder->getValue(bluetooth::Settings::State));
            auto event   = std::make_unique<BluetoothStatusRequestEvent>(state);
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
                worker->run();
                break;
            case BluetoothMessage::Scan:
                sendWorkerCommand(bluetooth::StartScan);
                break;
            case BluetoothMessage::StopScan:
                sendWorkerCommand(bluetooth::StopScan);
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
                sendWorkerCommand(bluetooth::StartPan);
                //                    }
            } break;
            case BluetoothMessage::Visible: {
                static bool visibility = true;
                sendWorkerCommand(visibility ? bluetooth::VisibilityOn : bluetooth::VisibilityOff);
                visibility = !visibility;
            } break;

            case BluetoothMessage::Play:
                sendWorkerCommand(bluetooth::ConnectAudio);
                break;
            case BluetoothMessage::StopPlayback:
                sendWorkerCommand(bluetooth::DisconnectAudio);
                break;

            default:
                break;
            }
            break;
        }
        case MessageType::BluetoothAddrResult: {
            auto addrMsg = static_cast<BluetoothAddrMessage *>(msg);
            worker->setDeviceAddress(addrMsg->addr);
        } break;
        case MessageType::BluetoothRequestStream: {
            auto result =
                std::make_shared<BluetoothRequestStreamResultMessage>(worker->currentProfile->getStreamData());
            bus.sendUnicast(std::move(result), "ServiceAudio");
            LOG_INFO("Queues sent after a request!");
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
