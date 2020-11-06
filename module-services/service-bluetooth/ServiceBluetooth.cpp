// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceBluetooth.hpp"

#include <log/log.hpp>                // for LOG_INFO, LOG_ERROR
#include <module-sys/Service/Bus.hpp> // for Bus
#include <bits/exception.h>           // for exception
#include <utility>                    // for move

#include "Constants.hpp"       // for bluetooth
#include "Service/Service.hpp" // for Service
#include "Service/Message.hpp" // for ResponseMessage, MessagePointer, DataMessage
#include "MessageType.hpp" // for MessageType, MessageType::BluetoothAddrResult, MessageType::BluetoothRequest, MessageType::BluetoothRequestStream
#include "messages/BluetoothMessage.hpp" // for BluetoothMessage, BluetoothRequestStreamResultMessage, BluetoothAddrMessage, BluetoothMessage::PAN, BluetoothMessage::Play, BluetoothMessage::Scan, BluetoothMessage::Start, BluetoothMessage::Stop, BluetoothMessage::StopScan, BluetoothMessage::Visible
#include "BluetoothWorker.hpp"           // for BluetoothWorker
#include "interface/profiles/Profile.hpp" // for Profile

ServiceBluetooth::ServiceBluetooth() : sys::Service(service::name::bluetooth)
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
    LOG_ERROR("Bluetooth experimental!");
    worker = std::make_unique<BluetoothWorker>(this);

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
            LOG_INFO("BT not handled!");
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
