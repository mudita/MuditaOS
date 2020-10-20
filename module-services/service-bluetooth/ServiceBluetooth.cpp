// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceBluetooth.hpp"
#include "Constants.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "MessageType.hpp"
#include "messages/BluetoothMessage.hpp"
#include <log/log.hpp>
#include <service-lwip/ServiceLwIP.hpp>

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

sys::Message_t ServiceBluetooth::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
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
                worker->stop_scan();
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
                worker->set_visible();
                break;

            case BluetoothMessage::Play:
                worker->play_audio();
                break;
            case BluetoothMessage::Stop:
                worker->stop_audio();
                break;

            default:
                break;
            }
            break;
        }
        default:
            LOG_INFO("BT not handled!");
            break;
        }
    }
    catch (std::exception &ex) {
        LOG_ERROR("Exception on BtService!: %s", ex.what());
    }
    if (dynamic_cast<BluetoothAddrMessage *>(msg) != nullptr) {
        auto addrMsg = static_cast<BluetoothAddrMessage *>(msg);
        worker->set_addr(addrMsg->addr);
    }

    return std::make_shared<sys::ResponseMessage>();
}

sys::ReturnCodes ServiceBluetooth::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_ERROR("TODO");
    return sys::ReturnCodes::Success;
}
