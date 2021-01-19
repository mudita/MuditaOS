// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Constants.hpp"
#include "service-bluetooth/ServiceBluetooth.hpp"
#include "service-bluetooth/BluetoothMessage.hpp"

#include <module-sys/Service/Bus.hpp>

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

ServiceBluetooth::ServiceBluetooth() : sys::Service(service::name::bluetooth)
{
    auto settings  = std::make_unique<settings::Settings>(this);
    settingsHolder = std::make_shared<Bluetooth::SettingsHolder>(std::move(settings));
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

    connect(message::bluetooth::RequestBondedDevices(), [&](sys::Message *msg) {
        auto bondedDevicesStr =
            std::visit(Bluetooth::StringVisitor(), this->settingsHolder->getValue(Bluetooth::Settings::BondedDevices));

        return std::make_shared<message::bluetooth::ResponseBondedDevices>(
            SettingsSerializer::fromString(bondedDevicesStr));
    });

    connect(message::bluetooth::RequestStatus(), [&](sys::Message *msg) {
        BluetoothStatus status;

        auto state = std::visit(Bluetooth::IntVisitor(), settingsHolder->getValue(Bluetooth::Settings::State));
        auto visibility =
            std::visit(Bluetooth::BoolVisitor(), settingsHolder->getValue(Bluetooth::Settings::Visibility));
        status.state      = static_cast<BluetoothStatus::State>(state);
        status.visibility = visibility;

        return std::make_shared<message::bluetooth::ResponseStatus>(status);
    });

    connect(typeid(message::bluetooth::SetStatus), [&](sys::Message *msg) {
        auto setStatusMsg = static_cast<message::bluetooth::SetStatus *>(msg);
        auto btStatus     = setStatusMsg->getStatus();
        worker->setVisibility(btStatus.visibility);

        switch (btStatus.state) {
        case BluetoothStatus::State::On:
            worker->run();
            break;
        case BluetoothStatus::State::Off:
            // TODO
            break;
        default:
            break;
        }

        return std::make_shared<message::bluetooth::ResponseStatus>(btStatus);
    });

    settingsHolder->onStateChange = [this]() {
        auto initialState =
            std::visit(Bluetooth::IntVisitor(), this->settingsHolder->getValue(Bluetooth::Settings::State));
        if (static_cast<BluetoothStatus::State>(initialState) == BluetoothStatus::State::On) {
            this->worker->run();
        }
    };

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceBluetooth::DeinitHandler()
{

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
                if (worker->scan()) {
                    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Success);
                }
                else {
                    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Failure);
                }
            case BluetoothMessage::StopScan:
                sendWorkerCommand(Bt::StopScan);
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
                worker->start_pan();
                //                    }
            } break;
            case BluetoothMessage::Visible: {
                static bool visibility = true;
                worker->setVisibility(visibility);
                visibility = !visibility;
            } break;

            case BluetoothMessage::Play:
                sendWorkerCommand(Bt::ConnectAudio);
                break;
            case BluetoothMessage::Stop:
                sendWorkerCommand(Bt::DisconnectAudio);
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
            sys::Bus::SendUnicast(std::move(result), "ServiceAudio", this);
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
void ServiceBluetooth::sendWorkerCommand(Bt::Command command)
{
    xQueueSend(workerQueue, &command, portMAX_DELAY);
}
