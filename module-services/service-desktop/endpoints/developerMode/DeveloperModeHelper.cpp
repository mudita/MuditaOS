// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeveloperModeHelper.hpp"
#include <service-desktop/DesktopMessages.hpp>
#include <parser/ParserUtils.hpp>

#include <service-desktop/parser/MessageHandler.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-cellular/CellularServiceAPI.hpp>

#include <gui/Common.hpp>
#include <service-appmgr/Actions.hpp>
#include <messages/AppMessage.hpp>

namespace parserFSM
{
    class Context;
} // namespace parserFSM

using namespace parserFSM;
auto DeveloperModeHelper::processPutRequest(Context &context) -> sys::ReturnCodes
{
    auto body = context.getBody();
    if (body[json::developerMode::keyPressed].is_number()) {
        auto keyValue = body[json::developerMode::keyPressed].int_value();
        auto state    = body[json::developerMode::state].int_value();
        sendKeypress(getKeyCode(keyValue), static_cast<gui::InputEvent::State>(state));
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
    else if (body[json::developerMode::AT].is_string()) {

        auto msg     = std::make_shared<cellular::RawCommand>();
        msg->command = body[json::developerMode::AT].string_value();
        msg->timeout = 3000;
        ownerServicePtr->bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
    }
    else if (body[json::developerMode::focus].bool_value()) {
        auto event = std::make_unique<sdesktop::developerMode::AppFocusChangeEvent>();
        auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        ownerServicePtr->bus.sendUnicast(std::move(msg), service::name::evt_manager);
    }
    else if (body[json::developerMode::isLocked].bool_value()) {
        auto event = std::make_unique<sdesktop::developerMode::ScreenlockCheckEvent>();
        auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        ownerServicePtr->bus.sendUnicast(std::move(msg), "ApplicationDesktop");
    }
    else if (body[json::developerMode::btState].bool_value()) {

        auto event = std::make_unique<sdesktop::developerMode::BluetoothStatusRequestEvent>();
        auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        ownerServicePtr->bus.sendUnicast(std::move(msg), "ServiceBluetooth");
    }
    else if (auto state = body[json::developerMode::btCommand].string_value(); !state.empty()) {
        BluetoothMessage::Request request;
        if (state == json::developerMode::btOn) {
            request = BluetoothMessage::Request::Start;
            LOG_INFO("turning on BT from harness!");
        }
        else {
            request = BluetoothMessage::Request::StopPlayback;
            LOG_INFO("turning off BT from harness!");
        }
        std::shared_ptr<BluetoothMessage> msg = std::make_shared<BluetoothMessage>(request);
        ownerServicePtr->bus.sendUnicast(std::move(msg), "ServiceBluetooth");

        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
    else if (body[json::developerMode::changeSim].is_number()) {
        int simSelected = body[json::developerMode::changeSim].int_value();
        requestSimChange(simSelected);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
    else {
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
    return sys::ReturnCodes::Unresolved;
}

auto DeveloperModeHelper::processGetRequest(Context &context) -> sys::ReturnCodes
{
    auto body = context.getBody();
    if (body[json::developerMode::getInfo].is_string()) {
        auto keyValue = body[json::developerMode::getInfo].string_value();
        if (keyValue == json::developerMode::simStateInfo) {
            context.setResponseBody(json11::Json::object(
                {{json::selectedSim, std::to_string(static_cast<int>(Store::GSM::get()->selected))},
                 {json::sim, std::to_string(static_cast<int>(Store::GSM::get()->sim))},
                 {json::trayState, std::to_string(static_cast<int>(Store::GSM::get()->tray))}}));
        }
        else {
            context.setResponseStatus(http::Code::BadRequest);
        }
    }
    else {
        context.setResponseStatus(http::Code::BadRequest);
    }
    MessageHandler::putToSendQueue(context.createSimpleResponse());
    return sys::ReturnCodes::Unresolved;
}

auto DeveloperModeHelper::getKeyCode(int val) noexcept -> bsp::KeyCodes
{
    switch (val) {
    case 0:
        return bsp::KeyCodes::NumericKey0;
    case 1:
        return bsp::KeyCodes::NumericKey1;
    case 2:
        return bsp::KeyCodes::NumericKey2;
    case 3:
        return bsp::KeyCodes::NumericKey3;
    case 4:
        return bsp::KeyCodes::NumericKey4;
    case 5:
        return bsp::KeyCodes::NumericKey5;
    case 6:
        return bsp::KeyCodes::NumericKey6;
    case 7:
        return bsp::KeyCodes::NumericKey7;
    case 8:
        return bsp::KeyCodes::NumericKey8;
    case 9:
        return bsp::KeyCodes::NumericKey9;
    case '*':
        return bsp::KeyCodes::NumericKeyAst;
    case '#':
        return bsp::KeyCodes::NumericKeyPnd;
    case 'a':
        return bsp::KeyCodes::JoystickLeft;
    case 'd':
        return bsp::KeyCodes::JoystickRight;
    case 'w':
        return bsp::KeyCodes::JoystickUp;
    case 's':
        return bsp::KeyCodes::JoystickDown;
    case '\n':
        return bsp::KeyCodes::JoystickEnter;
    case 11:
        return bsp::KeyCodes::FnLeft;
    case 12:
        return bsp::KeyCodes::FnRight;
    case 13:
        return bsp::KeyCodes::VolUp;
    case 14:
        return bsp::KeyCodes::VolDown;
    case 15:
        return bsp::KeyCodes::Torch;
    case 16:
        return bsp::KeyCodes::SSwitchUp;
    case 17:
        return bsp::KeyCodes::SSwitchDown;
    case 18:
        return bsp::KeyCodes::SSwitchMid;
    default:
        LOG_ERROR("invalid keycode");
        return bsp::KeyCodes::Undefined;
    };
}

void DeveloperModeHelper::sendKeypress(bsp::KeyCodes keyCode, gui::InputEvent::State state)
{
    RawKey key{.state = RawKey::State::Released, .key_code = keyCode};

    gui::InputEvent event(key, state, static_cast<gui::KeyCode>(keyCode));
    LOG_INFO("Sending %s", event.str().c_str());
    auto message = std::make_shared<app::AppInputEventMessage>(std::move(event));

    ownerServicePtr->bus.sendUnicast(std::move(message), service::name::evt_manager);
}

void DeveloperModeHelper::requestSimChange(const int simSelected)
{
    Store::GSM::SIM sim = Store::GSM::SIM::SIM1;
    if (simSelected == static_cast<int>(Store::GSM::SIM::SIM2)) {
        sim = Store::GSM::SIM::SIM2;
    }
    CellularServiceAPI::SetSimCard(ownerServicePtr, sim);
}
