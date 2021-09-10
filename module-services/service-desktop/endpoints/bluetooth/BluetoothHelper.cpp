// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/bluetooth/BluetoothHelper.hpp>
#include <endpoints/bluetooth/BluetoothEventMessages.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <endpoints/message/Sender.hpp>
#include <service-bluetooth/messages/SetStatus.hpp>
#include <service-desktop/DeveloperModeMessage.hpp>

#include <service-bluetooth/service-bluetooth/messages/Connect.hpp>
#include <service-bluetooth/service-bluetooth/messages/Disconnect.hpp>
#include <service-bluetooth/service-bluetooth/messages/Status.hpp>
#include <service-bluetooth/service-bluetooth/messages/BondedDevices.hpp>
#include <service-bluetooth/service-bluetooth/messages/Unpair.hpp>

namespace sdesktop::endpoints
{
    using sender::putToSendQueue;

    namespace btConsts = json::bluetooth;

    auto BluetoothHelper::processPutRequest(Context &context) -> sys::ReturnCodes
    {
        auto body = context.getBody();
        std::unique_ptr<sys::Message> msg;
        if (auto command = body[btConsts::command].string_value(); !command.empty()) {
            if (command == btConsts::commands::scanOn) {
                msg = std::make_unique<BluetoothMessage>(BluetoothMessage::Request::Scan);
            }
            else if (command == btConsts::commands::scanOff) {
                msg = std::make_unique<BluetoothMessage>(BluetoothMessage::Request::StopScan);
            }
            else if (command == btConsts::commands::changeVisibility) {
                msg = std::make_unique<BluetoothMessage>(BluetoothMessage::Request::Visible);
            }
        }
        else if (auto state = body[btConsts::state].object_items(); !state.empty()) {
            BluetoothStatus status{};
            const auto &power = state[btConsts::states::power];
            if (power == json::bluetooth::on) {
                status.state = BluetoothStatus::State::On;
                LOG_INFO("turning on BT from harness!");
            }
            else if (power == json::bluetooth::off) {
                status.state = BluetoothStatus::State::Off;
                LOG_INFO("turning off BT from harness!");
            }
            const auto &visibility = state[btConsts::states::visibility];
            status.visibility      = visibility == json::bluetooth::on;

            ::message::bluetooth::SetStatus setStatus(status);
            msg = std::make_unique<::message::bluetooth::SetStatus>(std::move(setStatus));
        }

        sendRequest(context, std::move(msg));
        putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Unresolved;
    }
    auto BluetoothHelper::processGetRequest(Context &context) -> sys::ReturnCodes
    {
        auto body = context.getBody();
        std::unique_ptr<sys::Message> msg;
        if (body[json::bluetooth::state].bool_value()) {
            msg = std::make_unique<::message::bluetooth::RequestStatus>();
        }
        else if (auto devices = body[json::bluetooth::devices].string_value(); !devices.empty()) {
            if (devices == json::bluetooth::devicesValues::scanned) {
                auto event = std::make_unique<sdesktop::bluetooth::GetAvailableDevicesEvent>();
                msg        = std::make_unique<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
            }
            else if (devices == json::bluetooth::devicesValues::bonded) {
                msg = std::make_unique<::message::bluetooth::RequestBondedDevices>();
            }
        }

        sendRequest(context, std::move(msg));
        return sys::ReturnCodes::Unresolved;
    }

    auto BluetoothHelper::processPostRequest(Context &context) -> sys::ReturnCodes
    {
        auto body = context.getBody();
        std::unique_ptr<sys::Message> msg;
        if (auto address = body[json::bluetooth::connect].string_value(); !address.empty()) {
            auto device = Devicei("DevMode device");
            sscanf_bd_addr(address.c_str(), device.address);
            msg = std::make_unique<::message::bluetooth::Connect>(device);
        }
        else if (auto address = body[json::bluetooth::pair].string_value(); !address.empty()) {
            auto device = Devicei("DevMode device");
            sscanf_bd_addr(address.c_str(), device.address);
            LOG_INFO("Requesting pairing form harness");
            msg = std::make_unique<BluetoothPairMessage>(device);
        }

        sendRequest(context, std::move(msg));
        putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Unresolved;
    }

    auto BluetoothHelper::processDeleteRequest(Context &context) -> sys::ReturnCodes
    {
        auto body = context.getBody();
        std::unique_ptr<sys::Message> msg;
        if (auto command = body[json::bluetooth::command].string_value(); !command.empty()) {
            if (command == json::bluetooth::commands::disconnect) {
                msg = std::make_unique<::message::bluetooth::Disconnect>();
            }
        }
        else if (auto address = body[json::bluetooth::unpair].string_value(); !address.empty()) {
            LOG_INFO("Requesting pairing form harness");
            auto device = Devicei("DevMode device");
            sscanf_bd_addr(address.c_str(), device.address);
            msg = std::make_unique<::message::bluetooth::Unpair>(device);
        }
        sendRequest(context, std::move(msg));
        putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Unresolved;
    }
    void BluetoothHelper::sendRequest(Context &context, std::unique_ptr<sys::Message> msg)
    {
        if (msg != nullptr) {
            ownerServicePtr->bus.sendUnicast(std::move(msg), "ServiceBluetooth");
        }
        else {
            LOG_ERROR("No valid request created, returning simpleResponse ...");
            putToSendQueue(context.createSimpleResponse());
        }
    }

} // namespace sdesktop::endpoints
