// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeveloperModeHelper.hpp"
#include "service-db/SettingsMessages.hpp"
#include <service-desktop/DesktopMessages.hpp>
#include <parser/ParserUtils.hpp>

#include <service-evtmgr/Constants.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <service-cellular-api>

#include <gui/Common.hpp>
#include <service-appmgr/Actions.hpp>
#include <messages/AppMessage.hpp>

#include <module-sys/SystemManager/messages/TetheringStateRequest.hpp>
#include <module-sys/SystemManager/Constants.hpp>

#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-desktop/parser/MessageHandler.hpp>
#include <service-desktop/endpoints/developerMode/event/ATRequest.hpp>
#include <service-appmgr/Controller.hpp>

#include <ctime>
#include <locks/data/PhoneLockMessages.hpp>

namespace parserFSM
{
    class Context;
} // namespace parserFSM

using namespace parserFSM;

namespace
{

    auto toTetheringState(const std::string &state) -> sys::phone_modes::Tethering
    {
        return state == json::developerMode::tetheringOn ? sys::phone_modes::Tethering::On
                                                         : sys::phone_modes::Tethering::Off;
    }
} // namespace

auto DeveloperModeHelper::processPut(Context &context) -> ProcessResult
{
    auto body = context.getBody();
    auto code = http::Code::BadRequest;
    if (body[json::developerMode::keyPressed].is_number()) {
        auto keyValue = body[json::developerMode::keyPressed].int_value();
        auto state    = body[json::developerMode::state].int_value();
        sendKeypress(getKeyCode(keyValue), static_cast<gui::InputEvent::State>(state));
        app::manager::Controller::preventBlockingDevice(owner);
        return {sent::no, std::nullopt};
    }
    else if (body[json::developerMode::AT].is_string()) {
        using namespace sdesktop::developerMode;
        auto cmd     = body[json::developerMode::AT].string_value();
        auto timeout = std::chrono::milliseconds(body[json::developerMode::timeout].int_value());
        LOG_DEBUG("at request sent with timeout >%d<", int(timeout.count()));
        auto event = std::make_unique<ATResponseEvent>(cmd, timeout);
        auto msg   = std::make_shared<DeveloperModeRequest>(std::move(event));
        code       = toCode(owner->bus.sendUnicast(msg, ServiceCellular::serviceName));
        return {sent::delayed, std::nullopt};
    }
    else if (body[json::developerMode::focus].bool_value()) {
        auto event = std::make_unique<sdesktop::developerMode::AppFocusChangeEvent>();
        auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        code       = toCode(owner->bus.sendUnicast(std::move(msg), service::name::evt_manager));
        return {sent::delayed, std::nullopt};
    }
    else if (body[json::developerMode::phoneLocked].bool_value()) {
        auto event = std::make_unique<sdesktop::developerMode::ScreenlockCheckEvent>();
        auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        code       = toCode(owner->bus.sendUnicast(std::move(msg), "ApplicationManager"));
        return {sent::delayed, std::nullopt};
    }
    else if (body[json::developerMode::changeAutoLockTimeout].is_string()) {
        auto value = body[json::developerMode::changeAutoLockTimeout].string_value();
        settings::EntryPath path;
        path.variable = settings::SystemProperties::autoLockTimeInSec;
        path.service  = service::name::db;
        path.scope    = settings::SettingsScope::Global;
        auto msg      = std::make_shared<settings::Messages::SetVariable>(std::move(path), std::move(value));
        code          = owner->bus.sendUnicast(std::move(msg), service::name::db) ? http::Code::NoContent
                                                                                  : http::Code::InternalServerError;

        return {sent::no, endpoint::ResponseContext{.status = code}};
    }
    else if (body[json::developerMode::changeSim].is_number()) {
        int simSelected = body[json::developerMode::changeSim].int_value();
        requestSimChange(simSelected);
        code = http::Code::NoContent;
        return {sent::no, endpoint::ResponseContext{.status = code}};
    }
    else if (body[json::developerMode::changeCellularStateCmd].is_number()) {
        int cellularState = body[json::developerMode::changeCellularStateCmd].int_value();
        code = requestCellularPowerStateChange(cellularState) ? http::Code::NoContent : http::Code::InternalServerError;

        return {sent::no, endpoint::ResponseContext{.status = code}};
    }
    else if (body[json::developerMode::smsCommand].is_string()) {
        if (body[json::developerMode::smsCommand].string_value() == json::developerMode::smsAdd) {
            const auto smsType = static_cast<SMSType>(context.getBody()[json::messages::messageType].int_value());
            if (smsType == SMSType::DRAFT || smsType == SMSType::QUEUED || smsType == SMSType::FAILED) {
                return prepareSMS(context);
            }
            else {
                return {sent::no, endpoint::ResponseContext{.status = http::Code::NotAcceptable}};
            }
        }
    }
    else if (body[json::developerMode::tethering].is_string()) {
        const auto &tetheringState = body[json::developerMode::tethering].string_value();
        owner->bus.sendUnicast(std::make_shared<sys::TetheringStateRequest>(toTetheringState(tetheringState)),
                               service::name::system_manager);
        return {sent::delayed, std::nullopt};
    }
    else if (body[json::developerMode::phoneLockCodeEnabled].is_bool()) {
        auto phoneLockState = body[json::developerMode::phoneLockCodeEnabled].bool_value();
        auto msg            = std::make_shared<locks::ExternalPhoneLockAvailabilityChange>(phoneLockState);
        code                = owner->bus.sendUnicast(std::move(msg), "ApplicationManager") ? http::Code::NoContent
                                                                                           : http::Code::InternalServerError;
    }
    else if (auto switchData = body[json::developerMode::switchApplication].object_items(); !switchData.empty()) {
        auto msg = std::make_shared<app::manager::SwitchRequest>(
            owner->GetName(),
            switchData[json::developerMode::switchData::applicationName].string_value(),
            switchData[json::developerMode::switchData::windowName].string_value(),
            nullptr);
        code = owner->bus.sendUnicast(std::move(msg), "ApplicationManager") ? http::Code::NoContent
                                                                            : http::Code::InternalServerError;
    }
    else if (auto switchData = body[json::developerMode::switchWindow].object_items(); !switchData.empty()) {
        auto msg = std::make_shared<app::AppSwitchWindowMessage>(
            switchData[json::developerMode::switchData::windowName].string_value(), "", nullptr);
        code = owner->bus.sendUnicast(std::move(msg),
                                      switchData[json::developerMode::switchData::applicationName].string_value())
                   ? http::Code::NoContent
                   : http::Code::InternalServerError;
    }

    else {
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
    return {sent::no, endpoint::ResponseContext{.status = code}};
}

auto DeveloperModeHelper::processGet(Context &context) -> ProcessResult
{
    auto body = context.getBody();
    if (body[json::developerMode::getInfo].is_string()) {
        auto keyValue = body[json::developerMode::getInfo].string_value();
        if (keyValue == json::developerMode::simStateInfo) {
            auto response = endpoint::ResponseContext{
                .body = json11::Json::object(
                    {{json::selectedSim, std::to_string(static_cast<int>(Store::GSM::get()->selected))},
                     {json::sim, std::to_string(static_cast<int>(Store::GSM::get()->sim))},
                     {json::trayState, std::to_string(static_cast<int>(Store::GSM::get()->tray))}})};
            response.status = http::Code::OK;
            return {sent::no, std::move(response)};
        }
        else if (keyValue == json::developerMode::cellularStateInfo) {
            if (!requestServiceStateInfo(owner)) {
                return {sent::no, endpoint::ResponseContext{.status = http::Code::NotAcceptable}};
            }
            else {
                return {sent::delayed, std::nullopt};
            }
        }
        else if (keyValue == json::developerMode::cellularSleepModeInfo) {
            if (!requestCellularSleepModeInfo(owner)) {
                return {sent::no, endpoint::ResponseContext{.status = http::Code::NotAcceptable}};
            }
            else {
                return {sent::delayed, std::nullopt};
            }
        }
        else {
            return {sent::no, endpoint::ResponseContext{.status = http::Code::BadRequest}};
        }
    }
    else {
        return {sent::no, endpoint::ResponseContext{.status = http::Code::BadRequest}};
    }
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

bool DeveloperModeHelper::sendKeypress(bsp::KeyCodes keyCode, gui::InputEvent::State state)
{
    RawKey key{.state = RawKey::State::Released, .keyCode = keyCode};

    gui::InputEvent event(key, state, static_cast<gui::KeyCode>(keyCode));
    auto message = std::make_shared<app::AppInputEventMessage>(event);

    return owner->bus.sendUnicast(std::move(message), service::name::evt_manager);
}

void DeveloperModeHelper::requestSimChange(const int simSelected)
{
    auto arg = (simSelected == static_cast<int>(cellular::api::SimSlot::SIM2)) ? cellular::api::SimSlot::SIM2
                                                                               : cellular::api::SimSlot::SIM1;
    owner->bus.sendUnicast<cellular::msg::request::sim::SetActiveSim>(arg);
}

bool DeveloperModeHelper::requestCellularPowerStateChange(const int cellularState)
{
    bool res = false;
    if (cellularState == 1) {
        res = CellularServiceAPI::ChangeModulePowerState(owner, cellular::service::State::PowerState::Off);
    }
    else if (cellularState == 2) {
        res = CellularServiceAPI::ChangeModulePowerState(owner, cellular::service::State::PowerState::On);
    }
    else if (cellularState == 3) {
        auto event = std::make_unique<sdesktop::developerMode::CellularHotStartEvent>();
        auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        res        = owner->bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
    }
    return res;
}
auto DeveloperModeHelper::smsRecordFromJson(const json11::Json &msgJson) -> SMSRecord
{
    auto record = SMSRecord();

    record.type = static_cast<SMSType>(msgJson[json::messages::messageType].int_value());
    utils::PhoneNumber phoneNumber(msgJson[json::messages::phoneNumber].string_value());
    record.number = phoneNumber.getView();
    record.date   = std::time(nullptr);
    record.body   = UTF8(msgJson[json::messages::messageBody].string_value());
    return record;
}

auto DeveloperModeHelper::prepareSMS(Context &context) -> ProcessResult
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

    DBServiceAPI::AddSMS(owner, record, std::move(listener));
    // actual success / fail happens in listener
    return {sent::delayed, std::nullopt};
}

bool DeveloperModeHelper::requestServiceStateInfo(sys::Service *serv)
{
    auto event = std::make_unique<sdesktop::developerMode::CellularStateInfoRequestEvent>();
    auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
    return serv->bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
}

bool DeveloperModeHelper::requestCellularSleepModeInfo(sys::Service *serv)
{
    auto event = std::make_unique<sdesktop::developerMode::CellularSleepModeInfoRequestEvent>();
    auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
    return serv->bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
}
