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

#include <log/log.hpp>

#include <bits/exception.h>
#include <utility>

ServiceBluetooth::ServiceBluetooth() : sys::Service(service::name::bluetooth)
{
    auto settings  = std::make_unique<settings::Settings>(this);
    settingsHolder = std::make_unique<Bluetooth::SettingsHolder>(std::move(settings));
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
    btStatus.state      = BluetoothStatus::BluetoothState::On;
    btStatus.visibility = true;
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceBluetooth::DeinitHandler()
{

    return sys::ReturnCodes::Success;
}

sys::MessagePointer ServiceBluetooth::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    // mock response on message::bluetooth::RequestStatus
    if (auto requestStatusMsg = dynamic_cast<message::bluetooth::RequestStatus *>(msg); nullptr != requestStatusMsg) {
        sys::Bus::SendUnicast(std::make_shared<message::bluetooth::ResponseStatus>(btStatus), msg->sender, this);
    }

    // temporary solution for handling message::bluetooth::SetStatus
    if (auto setStatusMsg = dynamic_cast<message::bluetooth::SetStatus *>(msg); nullptr != setStatusMsg) {
        btStatus = setStatusMsg->getStatus();
        sys::Bus::SendBroadcast(std::make_shared<message::bluetooth::ResponseStatus>(btStatus), this);
    }

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
                worker->stopScan();
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
            case BluetoothMessage::Visible:
                worker->toggleVisibility();
                break;

            case BluetoothMessage::Play:
                worker->establishAudioConnection();
                break;
            case BluetoothMessage::Stop:
                worker->disconnectAudioConnection();
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

