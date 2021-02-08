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

#include <service-db/DBServiceAPI.hpp>
#include <time/time_conversion.hpp>

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
    else if (body[json::developerMode::changeSim].is_number()) {
        int simSelected = body[json::developerMode::changeSim].int_value();
        requestSimChange(simSelected);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
    else if (body[json::developerMode::changeCellularStateCmd].is_number()) {
        int cellularState = body[json::developerMode::changeCellularStateCmd].int_value();
        if (requestCellularPowerStateChange(cellularState) == false) {
            context.setResponseStatus(http::Code::NotAcceptable);
        }
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
    else if (body[json::developerMode::smsCommand].is_string()) {
        if (body[json::developerMode::smsCommand].string_value() == json::developerMode::smsAdd) {
            const auto smsType = static_cast<SMSType>(context.getBody()[json::messages::messageType].int_value());
            if (smsType == SMSType::DRAFT || smsType == SMSType::QUEUED || smsType == SMSType::FAILED) {
                return prepareSMS(context);
            }
            else {
                context.setResponseStatus(http::Code::NotAcceptable);
                MessageHandler::putToSendQueue(context.createSimpleResponse());
            }
        }
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
            MessageHandler::putToSendQueue(context.createSimpleResponse());
        }
        else if (keyValue == json::developerMode::cellularStateInfo) {
            if (requestServiceStateInfo(ownerServicePtr) == false) {
                context.setResponseStatus(http::Code::NotAcceptable);
                MessageHandler::putToSendQueue(context.createSimpleResponse());
            }
        }
        else {
            context.setResponseStatus(http::Code::BadRequest);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
        }
    }
    else {
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }

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

bool DeveloperModeHelper::requestCellularPowerStateChange(const int cellularState)
{
    bool res = false;
    if (cellularState == 1) {
        res = CellularServiceAPI::ChangeModulePowerState(ownerServicePtr, cellular::State::PowerState::Off);
    }
    else if (cellularState == 2) {
        res = CellularServiceAPI::ChangeModulePowerState(ownerServicePtr, cellular::State::PowerState::On);
    }
    else if (cellularState == 3) {
        auto event = std::make_unique<sdesktop::developerMode::CellularHotStartEvent>();
        auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        res        = ownerServicePtr->bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
    }
    return res;
}
auto DeveloperModeHelper::smsRecordFromJson(json11::Json msgJson) -> SMSRecord
{
    auto record = SMSRecord();

    record.type = static_cast<SMSType>(msgJson[json::messages::messageType].int_value());
    record.date = utils::time::getCurrentTimestamp().getTime();
    record.body = UTF8(msgJson[json::messages::messageBody].string_value());
    return record;
}

auto DeveloperModeHelper::prepareSMS(Context &context) -> sys::ReturnCodes
{
    SMSRecord record = smsRecordFromJson(context.getBody());

    LOG_INFO("Adding sms of type %d to database", static_cast<int>(record.type));
    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            bool res = false;
            if (auto SMSAddResult = dynamic_cast<db::query::SMSAddResult *>(result)) {
                context.setResponseStatus(SMSAddResult->result ? http::Code::OK : http::Code::InternalServerError);
                MessageHandler::putToSendQueue(context.createSimpleResponse());
                LOG_INFO("Adding sms of type %d to database - %s",
                         static_cast<int>(record.type),
                         SMSAddResult->result ? "OK" : "NOK");
                res = true;
            }
            else {
                context.setResponseStatus(http::Code::InternalServerError);
                MessageHandler::putToSendQueue(context.createSimpleResponse());
            }
            return res;
        },
        context);

    DBServiceAPI::AddSMS(ownerServicePtr, record, std::move(listener));
    return sys::ReturnCodes::Success;
}

bool DeveloperModeHelper::requestServiceStateInfo(sys::Service *serv)
{
    auto event = std::make_unique<sdesktop::developerMode::CellularStateInfoRequestEvent>();
    auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
    return serv->bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
}
