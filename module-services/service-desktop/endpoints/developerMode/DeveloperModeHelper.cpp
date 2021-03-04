// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeveloperModeHelper.hpp"
#include <service-desktop/DesktopMessages.hpp>
#include <parser/ParserUtils.hpp>

#include <service-evtmgr/Constants.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-cellular/CellularServiceAPI.hpp>

#include <gui/Common.hpp>
#include <service-appmgr/Actions.hpp>
#include <messages/AppMessage.hpp>

#include <module-sys/SystemManager/messages/TetheringStateRequest.hpp>
#include <module-sys/SystemManager/Constants.hpp>

#include <service-db/DBServiceAPI.hpp>
#include <time/time_conversion.hpp>
#include <service-desktop/parser/MessageHandler.hpp>
#include <service-desktop/endpoints/developerMode/event/ATRequest.hpp>
#include <service-desktop/endpoints/developerMode/event/ApplicationManagerEvents.hpp>
#include <service-appmgr/service-appmgr/model/ApplicationManager.hpp>
#include <service-appmgr/service-appmgr/Controller.hpp>

namespace parserFSM
{
    class Context;
} // namespace parserFSM

using namespace parserFSM;

namespace
{

    auto toTetheringState(const std::string &state) -> sys::phone_modes::Tethering
    {
        return state == json::developerMode::tethering::on ? sys::phone_modes::Tethering::On
                                                           : sys::phone_modes::Tethering::Off;
    }
} // namespace

auto DeveloperModeHelper::processPut(Context &context) -> ProcessResult
{
    using namespace json::developerMode;
    const auto &body = context.getBody();

    if (body[cmd::keyPressed].is_number()) {
        return processPutKeyPressed(context);
    }
    else if (body[cmd::AT].is_string()) {
        return processPutAT(context);
    }
    else if (body[cmd::focus].bool_value()) {
        return processPutFocus();
    }
    else if (body[cmd::isLocked].bool_value()) {
        return processPutIsLocked();
    }
    else if (body[cmd::changeAutoLockTimeout].is_string()) {
        return processPutChangeAutoLockTimeout(context);
    }
    else if (body[cmd::changeSim].is_number()) {
        return processPutChangeSim(context);
    }
    else if (body[cmd::changeCellularState].is_number()) {
        return processPutChangeCellularState(context);
    }
    else if (body[cmd::sms].is_string()) {
        return processPutSms(context);
    }
    else if (body[cmd::tethering].is_string()) {
        return processPutTethering(context);
    }
    else if (body[cmd::usbSecurityStatus].is_string()) {
        return processPutUsbSecurity(context);
    }
    else if (body[cmd::appmgr].bool_value()) {
        return processPutAppMgr(context);
    }
    else {
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }

    return {sent::no, endpoint::ResponseContext{.status = http::Code::BadRequest}};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutKeyPressed(Context &context)
{
    using namespace json::developerMode;
    const auto &body    = context.getBody();
    const auto keyValue = body[cmd::keyPressed].int_value();
    const auto state    = body[cmd::state].int_value();
    sendKeypress(getKeyCode(keyValue), static_cast<gui::InputEvent::State>(state));
    app::manager::Controller::preventBlockingDevice(owner);
    return {sent::no, std::nullopt};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutAT(Context &context)
{
    using namespace json::developerMode;
    using namespace sdesktop::developerMode;
    const auto &body    = context.getBody();
    const auto &cmd     = body[cmd::AT].string_value();
    const auto &timeout = std::chrono::milliseconds(body[json::developerMode::at::timeout].int_value());
    LOG_DEBUG("at request send >%s\n< with timeout >%d<", cmd.c_str(), int(timeout.count()));
    auto event = std::make_unique<ATResponseEvent>(cmd, timeout);
    auto msg   = std::make_shared<DeveloperModeRequest>(std::move(event));
    owner->bus.sendUnicast(msg, ServiceCellular::serviceName);
    return {sent::delayed, std::nullopt};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutFocus()
{
    using namespace sdesktop::developerMode;
    auto event = std::make_unique<AppFocusChangeEvent>();
    auto msg   = std::make_shared<DeveloperModeRequest>(std::move(event));
    owner->bus.sendUnicast(std::move(msg), service::name::evt_manager);
    return {sent::delayed, std::nullopt};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutIsLocked()
{
    using namespace sdesktop::developerMode;
    auto event = std::make_unique<ScreenlockCheckEvent>();
    auto msg   = std::make_shared<DeveloperModeRequest>(std::move(event));
    owner->bus.sendUnicast(std::move(msg), "ApplicationDesktop");
    return {sent::delayed, std::nullopt};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutChangeAutoLockTimeout(Context &context)
{
    using namespace json::developerMode;
    const auto &body  = context.getBody();
    const auto &value = body[cmd::changeAutoLockTimeout].string_value();
    settings::EntryPath path;
    path.variable   = settings::SystemProperties::lockTime;
    path.service    = service::name::db;
    path.scope      = settings::SettingsScope::Global;
    auto msg        = std::make_shared<settings::Messages::SetVariable>(std::move(path), std::move(value));
    const auto code = toCode(owner->bus.sendUnicast(std::move(msg), service::name::db));
    return {sent::no, endpoint::ResponseContext{.status = code}};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutChangeSim(Context &context)
{
    using namespace json::developerMode;
    const auto &body      = context.getBody();
    const int simSelected = body[cmd::changeSim].int_value();
    requestSimChange(simSelected);
    return {sent::no, endpoint::ResponseContext{.status = toCode(true)}};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutChangeCellularState(Context &context)
{
    using namespace json::developerMode;
    const auto &body        = context.getBody();
    const int cellularState = body[cmd::changeCellularState].int_value();
    const auto code         = toCode(requestCellularPowerStateChange(cellularState));
    return {sent::no, endpoint::ResponseContext{.status = code}};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutSms(Context &context)
{
    using namespace json::developerMode;
    const auto &body = context.getBody();

    if (body[cmd::sms].string_value() != sms::smsAdd) {
        return {sent::no, endpoint::ResponseContext{.status = http::Code::BadRequest}};
    }

    const auto smsType = static_cast<SMSType>(body[json::messages::messageType].int_value());
    if (smsType == SMSType::DRAFT || smsType == SMSType::QUEUED || smsType == SMSType::FAILED) {
        return prepareSMS(context);
    }

    return {sent::no, endpoint::ResponseContext{.status = http::Code::NotAcceptable}};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutTethering(Context &context)
{
    using namespace json::developerMode;
    const auto &body           = context.getBody();
    const auto &tetheringState = body[cmd::tethering].string_value();
    owner->bus.sendUnicast(std::make_shared<sys::TetheringStateRequest>(toTetheringState(tetheringState)),
                           service::name::system_manager);
    return {sent::delayed, std::nullopt};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutUsbSecurity(Context &context)
{
    using namespace json::developerMode;
    const auto &body                      = context.getBody();
    std::shared_ptr<sys::DataMessage> msg = std::make_shared<sdesktop::usb::USBConfigured>();
    if (body[cmd::usbSecurityStatus].string_value() == usbSecurityStatus::unlocked) {
        msg = std::make_shared<sdesktop::passcode::ScreenPasscodeUnlocked>();
    }
    auto code = toCode(owner->bus.sendUnicast(std::move(msg), "ServiceDesktop"));

    return {sent::no, endpoint::ResponseContext{.status = code}};
}

BaseHelper::ProcessResult DeveloperModeHelper::processPutAppMgr(Context &context)
{
    using namespace sdesktop::developerMode;
    using namespace json::developerMode;
    const auto &body = context.getBody();

    if (body[appmgr::startApp].is_string()) {
        auto event = std::make_unique<ApplicationStartRequest>(body[appmgr::startApp].string_value());
        auto msg   = std::make_shared<DeveloperModeRequest>(std::move(event));
        owner->bus.sendUnicast(std::move(msg), app::manager::ApplicationManager::ServiceName);
        return {sent::delayed, std::nullopt};
    }
    else if (body[appmgr::switchBack].bool_value()) {
        auto event = std::make_unique<ApplicationSwitchBackRequest>();
        auto msg   = std::make_shared<DeveloperModeRequest>(std::move(event));
        owner->bus.sendUnicast(std::move(msg), app::manager::ApplicationManager::ServiceName);
        return {sent::delayed, std::nullopt};
    }

    return {sent::no, endpoint::ResponseContext{.status = http::Code::BadRequest}};
}

auto DeveloperModeHelper::processGet(Context &context) -> ProcessResult
{
    using namespace json::developerMode;
    const auto body = context.getBody();

    if (body[cmd::getInfo].is_string()) {
        return processGetInfo(context);
    }
    else if (body[cmd::appmgr].bool_value()) {
        return processGetAppMgr(context);
    }

    return {sent::no, endpoint::ResponseContext{.status = http::Code::BadRequest}};
}

BaseHelper::ProcessResult DeveloperModeHelper::processGetInfo(Context &context)
{
    using namespace json::developerMode;
    const auto body     = context.getBody();
    const auto keyValue = body[cmd::getInfo].string_value();

    if (keyValue == getInfo::simStateInfo) {
        auto response = endpoint::ResponseContext{
            .body = json11::Json::object(
                {{json::selectedSim, std::to_string(static_cast<int>(Store::GSM::get()->selected))},
                 {json::sim, std::to_string(static_cast<int>(Store::GSM::get()->sim))},
                 {json::trayState, std::to_string(static_cast<int>(Store::GSM::get()->tray))}})};
        response.status = http::Code::OK;
        return {sent::no, std::move(response)};
    }
    else if (keyValue == getInfo::cellularStateInfo) {
        if (requestServiceStateInfo(owner) == false) {
            return {sent::no, endpoint::ResponseContext{.status = http::Code::NotAcceptable}};
        }
    }

    return {sent::no, endpoint::ResponseContext{.status = http::Code::BadRequest}};
}

BaseHelper::ProcessResult DeveloperModeHelper::processGetAppMgr(Context &context)
{
    using namespace sdesktop::developerMode;
    using namespace json::developerMode;
    const auto body = context.getBody();

    if (body[appmgr::appStack].bool_value()) {
        auto event = std::make_unique<ApplicationStackRequest>();
        auto msg   = std::make_shared<DeveloperModeRequest>(std::move(event));
        owner->bus.sendUnicast(std::move(msg), app::manager::ApplicationManager::ServiceName);
        return {sent::delayed, std::nullopt};
    }

    return {sent::no, endpoint::ResponseContext{.status = http::Code::BadRequest}};
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
    RawKey key{.state = RawKey::State::Released, .key_code = keyCode};

    gui::InputEvent event(key, state, static_cast<gui::KeyCode>(keyCode));
    LOG_INFO("Sending %s", event.str().c_str());
    auto message = std::make_shared<app::AppInputEventMessage>(std::move(event));

    return owner->bus.sendUnicast(std::move(message), service::name::evt_manager);
}

void DeveloperModeHelper::requestSimChange(const int simSelected)
{
    Store::GSM::SIM sim = Store::GSM::SIM::SIM1;
    if (simSelected == static_cast<int>(Store::GSM::SIM::SIM2)) {
        sim = Store::GSM::SIM::SIM2;
    }
    CellularServiceAPI::SetSimCard(owner, sim);
}

bool DeveloperModeHelper::requestCellularPowerStateChange(const int cellularState)
{
    bool res = false;
    if (cellularState == 1) {
        res = CellularServiceAPI::ChangeModulePowerState(owner, cellular::State::PowerState::Off);
    }
    else if (cellularState == 2) {
        res = CellularServiceAPI::ChangeModulePowerState(owner, cellular::State::PowerState::On);
    }
    else if (cellularState == 3) {
        auto event = std::make_unique<sdesktop::developerMode::CellularHotStartEvent>();
        auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        res        = owner->bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
    }
    return res;
}
auto DeveloperModeHelper::smsRecordFromJson(json11::Json msgJson) -> SMSRecord
{
    auto record = SMSRecord();

    record.type = static_cast<SMSType>(msgJson[json::messages::messageType].int_value());
    utils::PhoneNumber phoneNumber(msgJson[json::messages::phoneNumber].string_value());
    record.number = phoneNumber.getView();
    record.date   = utils::time::getCurrentTimestamp().getTime();
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
