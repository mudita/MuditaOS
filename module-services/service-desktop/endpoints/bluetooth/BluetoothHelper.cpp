// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothHelper.hpp"
#include <service-desktop/DesktopMessages.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <parser/MessageHandler.hpp>
#include <service-bluetooth/messages/SetStatus.hpp>

auto parserFSM::BluetoothHelper::processPutRequest(parserFSM::Context &context) -> sys::ReturnCodes
{
    auto body = context.getBody();
    if (auto command = body[json::bluetooth::command].string_value(); !command.empty()) {
        BluetoothMessage::Request btRequest{};

        if (command == json::bluetooth::scanOn) {
            btRequest = BluetoothMessage::Request::Scan;
        }
        else if (command == json::bluetooth::scanOff) {
            btRequest = BluetoothMessage::Request::StopScan;
        }
        ownerServicePtr->bus.sendUnicast(std::make_shared<BluetoothMessage>(btRequest), "ServiceBluetooth");
    }
    else if (auto state = body[json::bluetooth::state].string_value(); !state.empty()) {
        BluetoothStatus status{};
        if (state == json::bluetooth::btOn) {
            status.state = BluetoothStatus::State::On;
            LOG_INFO("turning on BT from harness!");
        }
        else if (state == json::bluetooth::btOff) {
            status.state = BluetoothStatus::State::Off;
            LOG_INFO("turning off BT from harness!");
        }
        ::message::bluetooth::SetStatus setStatus(status);

        ownerServicePtr->bus.sendUnicast(std::make_shared<::message::bluetooth::SetStatus>(std::move(setStatus)),
                                         "ServiceBluetooth");
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }

    return sys::ReturnCodes::Unresolved;
}
auto parserFSM::BluetoothHelper::processGetRequest(parserFSM::Context &context) -> sys::ReturnCodes
{
    auto body = context.getBody();
    if (body[json::bluetooth::state].bool_value()) {

        auto event = std::make_unique<sdesktop::bluetooth::BluetoothStatusRequestEvent>();
        auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        ownerServicePtr->bus.sendUnicast(std::move(msg), "ServiceBluetooth");
    }
    return sys::ReturnCodes::Unresolved;
}
