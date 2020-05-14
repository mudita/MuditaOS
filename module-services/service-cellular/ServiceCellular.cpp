/*
 *  @file ServiceCellular.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 03.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <ctime>
#include <functional>
#include <iomanip>

#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "ServiceCellular.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

#include "MessageType.hpp"

#include "messages/CellularMessage.hpp"
#include <ticks.hpp>

#include "log/log.hpp"

#include "service-appmgr/ApplicationManager.hpp"
#include "service-appmgr/messages/APMMessage.hpp"
#include "service-cellular/SignalStrength.hpp"
#include "service-evtmgr/messages/EVMessages.hpp"
#include "ucs2/UCS2.hpp"

#include "service-db/api/DBServiceAPI.hpp"
#include "service-db/messages/DBNotificationMessage.hpp"

#include "service-evtmgr/api/EventManagerServiceAPI.hpp"

#include "time/time_conversion.hpp"
#include <Utils.hpp>
#include <at/URC_QIND.hpp>
#include <common_data/EventStore.hpp>
#include <service-evtmgr/Constants.hpp>

#include <vector>
#include <utility>
#include <optional>
#include <string>

const char *ServiceCellular::serviceName = "ServiceCellular";

inline const auto cellularStack = 24000UL;

using namespace cellular;

const char *State::c_str(State::ST state)
{
    switch (state) {
    case ST::Idle:
        return "Idle";
    case ST::StatusCheck:
        return "StatusCheck";
    case ST::PowerUpInProgress:
        return "PowerUpInProgress";
    case ST::PowerUpProcedure:
        return "PowerUpProcedure";
    case ST::AudioConfigurationProcedure:
        return "AudioConfigurationProcedure";
    case ST::ModemOn:
        return "ModemOn";
    case ST::SimSelect:
        return "SimSelect";
    case ST::Failed:
        return "Failed";
    case ST::SanityCheck:
        return "SanityCheck";
    case ST::SimInit:
        return "SimInit";
    case ST::ModemFatalFailure:
        return "ModemFatalFailure";
    case ST::CellularConfProcedure:
        return "CellularStartConfProcedure";
    }
    return "";
}

void State::set(ServiceCellular *owner, ST state)
{
    assert(owner);
    LOG_DEBUG("GSM state: (%s) -> (%s)", c_str(this->state), c_str(state));
    this->state = state;
    auto msg    = std::make_shared<StateChange>(state);
    sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceCellularNotifications, owner);
}

State::ST State::get() const
{
    return this->state;
}

ServiceCellular::ServiceCellular() : sys::Service(serviceName, "", cellularStack, sys::ServicePriority::Idle)
{

    LOG_INFO("[ServiceCellular] Initializing");

    busChannels.push_back(sys::BusChannels::ServiceCellularNotifications);
    busChannels.push_back(sys::BusChannels::ServiceDBNotifications);

    callStateTimerId = CreateTimer(Ticks::MsToTicks(1000), true);
    stateTimerId     = CreateTimer(Ticks::MsToTicks(1000), true);

    ongoingCall.setStartCallAction([=](const CalllogRecord &rec) {
        auto call = DBServiceAPI::CalllogAdd(this, rec);
        if (call.ID == DB_ID_NONE) {
            LOG_ERROR("CalllogAdd failed");
        }
        return call;
    });

    ongoingCall.setEndCallAction([=](const CalllogRecord &rec) { return DBServiceAPI::CalllogUpdate(this, rec); });

    notificationCallback = [this](std::vector<uint8_t> &data) {
        LOG_DEBUG("Notifications callback called with %u data bytes", static_cast<unsigned int>(data.size()));
        TS0710_Frame frame(data);
        std::string message;
        auto msg = identifyNotification(frame.getFrame().data);

        if (msg == std::nullopt) {
            LOG_INFO("Skipped uknown notification");
            return;
        }

        sys::Bus::SendMulticast(msg.value(), sys::BusChannels::ServiceCellularNotifications, this);
    };
}

ServiceCellular::~ServiceCellular()
{
    LOG_INFO("[ServiceCellular] Cleaning resources");
}

void ServiceCellular::CallStateTimerHandler()
{
    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularListCurrentCalls);
    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, this);
}

// Invoked when timer ticked
void ServiceCellular::TickHandler(uint32_t id)
{
    if (id == callStateTimerId) {
        CallStateTimerHandler();
    }
    else if (id == stateTimerId) {
        LOG_INFO("State timer tick");
        handleStateTimer();
    }
    else {
        LOG_ERROR("Unrecognized timer ID = %" PRIu32, id);
    }
}

sys::ReturnCodes ServiceCellular::InitHandler()
{
    board = EventManagerServiceAPI::GetBoard(this);
    switch (board) {
    case bsp::Board::T4:
        state.set(this, State::ST::StatusCheck);
        break;
    case bsp::Board::T3:
        state.set(this, State::ST::PowerUpProcedure);
        break;
    case bsp::Board::Linux:
        state.set(this, State::ST::PowerUpProcedure);
        break;
    default:
        return sys::ReturnCodes::Failure;
        break;
    }
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceCellular::DeinitHandler()
{

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceCellular::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[ServiceCellular] PowerModeHandler: %s", c_str(mode));

    switch (mode) {
    case sys::ServicePowerMode ::Active:
        // muxdaemon->ExitSleepMode();
        break;
    case sys::ServicePowerMode ::SuspendToRAM:
    case sys::ServicePowerMode ::SuspendToNVM:
        LOG_FATAL("TEMPORARY DISABLED!!!!! UNCOMMENT WHEN READY.");
        //            muxdaemon->EnterSleepMode();
        break;
    }

    return sys::ReturnCodes::Success;
}

void ServiceCellular::change_state(cellular::StateChange *msg)
{
    assert(msg);
    switch (msg->request) {
    case State::ST::Idle:
        handle_idle();
        break;
    case State::ST::StatusCheck:
        handle_status_check();
        break;
    case State::ST::PowerUpInProgress:
        handle_power_up_in_progress_procedure();
        break;
    case State::ST::PowerUpProcedure:
        handle_power_up_procedure();
        break;
    case State::ST::AudioConfigurationProcedure:
        handle_audio_conf_procedure();
        break;
    case State::ST::CellularConfProcedure:
        handle_start_conf_procedure();
        break;
    case State::ST::SanityCheck:
        handle_sim_sanity_check();
        break;
    case State::ST::SimInit:
        handle_sim_init();
        break;
    case State::ST::SimSelect:
        handle_select_sim();
        break;
    case State::ST::ModemOn:
        handle_modem_on();
        break;
    case State::ST::ModemFatalFailure:
        handle_fatal_failure();
        break;
    case State::ST::Failed:
        handle_failure();
        break;
    };
}

bool ServiceCellular::handle_idle()
{
    LOG_DEBUG("Idle");
    return true;
}

bool ServiceCellular::handle_power_up_procedure()
{
    switch (board) {
    case bsp::Board::T4: {
        LOG_DEBUG("T4 - cold start");
        cmux->TurnOnModem();
        state.set(this, State::ST::PowerUpInProgress);
        break;
    }
    case bsp::Board::T3: {
        // check baud once to determine if it's already turned on
        auto ret = cmux->BaudDetectOnce();
        if (ret == TS0710::ConfState::Success) {
            // it's on aka hot start.
            LOG_DEBUG("T3 - hot start");
            state.set(this, State::ST::CellularConfProcedure);
            break;
        }
        else {
            // it's off aka cold start
            LOG_DEBUG("T3 - cold start");
            cmux->TurnOnModem();
            // if it's T3, then wait for status pin to become active, to align its starting position with T4
            vTaskDelay(pdMS_TO_TICKS(8000));
            state.set(this, State::ST::PowerUpInProgress);
            break;
        }
    }
    case bsp::Board::Linux: {
        // it is basically the same as T3
        // check baud once to determine if it's already turned on
        auto ret = cmux->BaudDetectOnce();
        if (ret == TS0710::ConfState::Success) {
            // it's on aka hot start.
            LOG_DEBUG("Linux - hot start");
            state.set(this, State::ST::CellularConfProcedure);
            break;
        }
        else {
            // it's off aka cold start
            LOG_DEBUG("Linux - cold start");
            LOG_WARN("Press PWR_KEY for 2 sec on modem eval board!");
            vTaskDelay(pdMS_TO_TICKS(2000)); // give some 2 secs more for user input
            // if it's Linux (T3), then wait for status pin to become active, to align its starting position with T4
            vTaskDelay(pdMS_TO_TICKS(8000));
            state.set(this, State::ST::PowerUpInProgress);
            break;
        }
    }
    case bsp::Board::none:
    default:
        LOG_FATAL("Board not known!");
        assert(0);
        break;
    }
    return true;
}

bool ServiceCellular::handle_power_up_in_progress_procedure(void)
{
    auto ret = cmux->BaudDetectProcedure();
    if (ret == TS0710::ConfState::Success) {
        state.set(this, cellular::State::ST::CellularConfProcedure);
        return true;
    }
    else {
        state.set(this, cellular::State::ST::ModemFatalFailure);
        return false;
    }
}

bool ServiceCellular::handle_start_conf_procedure()
{
    // Start configuration procedure, if it's first run modem will be restarted
    auto confRet = cmux->ConfProcedure();
    if (confRet == TS0710::ConfState::Success) {
        state.set(this, State::ST::AudioConfigurationProcedure);
        return true;
    }
    state.set(this, State::ST::Failed);
    return false;
}

bool ServiceCellular::handle_audio_conf_procedure()
{
    auto audioRet = cmux->AudioConfProcedure();
    if (audioRet == TS0710::ConfState::Success) {
        auto cmd =
            at::factory(at::AT::IPR) + std::to_string(ATPortSpeeds_text[cmux->getStartParams().PortSpeed]) + "\r";
        LOG_DEBUG("Setting baudrate %i baud", ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
        if (!cmux->getParser()->cmd(cmd)) {
            LOG_ERROR("Baudrate setup error");
            state.set(this, State::ST::Failed);
            return false;
        }
        cmux->getCellular()->SetSpeed(ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
        vTaskDelay(1000);

        if (cmux->StartMultiplexer() == TS0710::ConfState::Success) {

            LOG_DEBUG("[ServiceCellular] Modem is fully operational");

            // open channel - notifications
            DLC_channel *notificationsChannel = cmux->get(TS0710::Channel::Notifications);
            if (notificationsChannel != nullptr) {
                LOG_DEBUG("Setting up notifications callback");
                notificationsChannel->setCallback(notificationCallback);
            }
            state.set(this, State::ST::SanityCheck);
            return true;
        }
        else {
            state.set(this, State::ST::Failed);
            return false;
        }
    }
    else if (audioRet == TS0710::ConfState::Failure) {
        /// restart
        state.set(this, State::ST::AudioConfigurationProcedure);
        return true;
    }
    // Reset procedure started, do nothing here
    state.set(this, State::ST::Idle);
    return true;
}

sys::Message_t ServiceCellular::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    std::shared_ptr<sys::ResponseMessage> responseMsg;

    switch (static_cast<MessageType>(msgl->messageType)) {
    case MessageType::CellularStateRequest: {
        change_state(dynamic_cast<cellular::StateChange *>(msgl));
        responseMsg = std::make_shared<CellularResponseMessage>(true);
    } break;
    case MessageType::CellularCall: {
        auto *msg = dynamic_cast<CellularCallMessage *>(msgl);
        assert(msg != nullptr);

        switch (msg->type) {
        case CellularCallMessage::Type::Ringing: {
            auto ret    = ongoingCall.startCall(msg->number, CallType::CT_OUTGOING);
            responseMsg = std::make_shared<CellularResponseMessage>(ret);
            break;
        } break;
        case CellularCallMessage::Type::IncomingCall: {
            auto ret = true;
            if (!ongoingCall.isValid()) {
                ret = ongoingCall.startCall(msg->number, CallType::CT_INCOMING);
            }
            responseMsg = std::make_shared<CellularResponseMessage>(ret);
            break;
        }
        }
    } break;
    // Incoming notifications from Notification Virtual Channel
    case MessageType::CellularNotification: {
        CellularNotificationMessage *msg = dynamic_cast<CellularNotificationMessage *>(msgl);
        if (msg != nullptr) {
            switch (msg->type) {
            case CellularNotificationMessage::Type::CallActive: {
                auto ret    = ongoingCall.setActive();
                responseMsg = std::make_shared<CellularResponseMessage>(ret);
                break;
            }
            case CellularNotificationMessage::Type::CallAborted: {
                stopTimer(callStateTimerId);
                auto ret    = ongoingCall.endCall();
                responseMsg = std::make_shared<CellularResponseMessage>(ret);
                break;
            }
            case CellularNotificationMessage::Type::PowerUpProcedureComplete: {
                if (board == bsp::Board::T3 || board == bsp::Board::Linux) {
                    state.set(this, State::ST::CellularConfProcedure);
                    responseMsg = std::make_shared<CellularResponseMessage>(true);
                }
                break;
            }
            case CellularNotificationMessage::Type::NewIncomingSMS: {
                LOG_INFO("New incoming sms received");
                receiveSMS(msg->data);
                responseMsg = std::make_shared<CellularResponseMessage>(true);
                break;
            }
            case CellularNotificationMessage::Type::RawCommand: {
                auto m       = dynamic_cast<cellular::RawCommand *>(msgl);
                auto channel = cmux->get(TS0710::Channel::Commands);
                if (!m || !channel) {
                    LOG_ERROR("RawCommand error: %s %s", m == nullptr ? "" : "bad cmd", !channel ? "no channel" : "");
                    break;
                }
                auto respMsg      = std::make_shared<cellular::RawCommandResp>(true);
                auto ret          = channel->cmd(m->command.c_str(), m->timeout);
                respMsg->response = ret.response;
                if (respMsg->response.size()) {
                    for (auto const &el : respMsg->response) {
                        LOG_DEBUG("> %s", el.c_str());
                    }
                }
                responseMsg = respMsg;
                break;
            } break;
            case CellularNotificationMessage::Type::SIM:
                if (Store::GSM::get()->tray == Store::GSM::Tray::IN) {
                    state.set(this, cellular::State::ST::SimInit);
                    responseMsg = std::make_shared<CellularResponseMessage>(true);
                }
                break;
            default: {
                LOG_INFO("Skipped CellularNotificationMessage::Type %d", static_cast<int>(msg->type));
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
            }
        }
    } break;

    case MessageType::CellularSimProcedure: {
        state.set(this, State::ST::SimSelect);
    } break;

    case MessageType::CellularListCurrentCalls: {
        constexpr size_t numberOfExpectedTokens = 3;
        auto ret                                = cmux->get(TS0710::Channel::Commands)->cmd(at::AT::CLCC);
        if (ret && ret.response.size() == numberOfExpectedTokens) {
            // TODO: alek: add case when more status calls is returned
            // TODO: alek: add cellular call validation and check it with modemcall

            // TODO: alek - just handle parts of response properly
            // if CellularListCurrentCalls is recieved after the call is aborted it will return 2 tokens instead of 3
            // this should be acceptable and hence warning instead of error is logged in such case
            bool retVal    = true;
            auto callEntry = ret.response[1];

            try {
                ModemCall::ModemCall call(callEntry);
                LOG_DEBUG("%s", utils::to_string(call).c_str());
                // If call changed to "Active" state stop callStateTimer(used for polling for call state)
                if (call.state == ModemCall::CallState::Active) {
                    auto msg =
                        std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallActive);
                    sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceCellularNotifications, this);
                    stopTimer(callStateTimerId);
                }
            }
            catch (const std::exception &e) {
                LOG_ERROR("exception \"%s\" was thrown", e.what());
                assert(0);
                retVal = false;
            }
            responseMsg = std::make_shared<CellularResponseMessage>(retVal);
        }
        else {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
    } break;

    case MessageType::CellularHangupCall: {
        auto channel = cmux->get(TS0710::Channel::Commands);
        LOG_INFO("CellularHangupCall");
        if (channel) {
            if (channel->cmd(at::AT::ATH)) {
                stopTimer(callStateTimerId);
                if (!ongoingCall.endCall(CellularCall::Forced::True)) {
                    LOG_ERROR("Failed to end ongoing call");
                }
                responseMsg = std::make_shared<CellularResponseMessage>(true, msgl->messageType);
            }
            else {
                LOG_ERROR("Call not aborted");
                responseMsg = std::make_shared<CellularResponseMessage>(false, msgl->messageType);
            }
            break;
        }
        responseMsg = std::make_shared<CellularResponseMessage>(false, msgl->messageType);
    } break;

    case MessageType::CellularAnswerIncomingCall: {
        auto channel = cmux->get(TS0710::Channel::Commands);
        auto ret     = false;
        if (channel) {
            // TODO alek: check if your request isn't for 5 sec when you wait in command for 90000, it's exclusivelly
            // set to 5000ms in command requesting...
            auto response = channel->cmd(at::AT::ATA);
            if (response) {
                // Propagate "CallActive" notification into system
                sys::Bus::SendMulticast(
                    std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallActive),
                    sys::BusChannels::ServiceCellularNotifications,
                    this);
                ret = true;
            }
        }
        responseMsg = std::make_shared<CellularResponseMessage>(ret);
    } break;

    case MessageType::CellularCallRequest: {
        auto *msg = dynamic_cast<CellularCallRequestMessage *>(msgl);
        assert(msg != nullptr);
        auto channel = cmux->get(TS0710::Channel::Commands);
        if (channel) {
            auto ret = channel->cmd(at::factory(at::AT::ATD) + msg->number.getEntered() + ";\r");
            if (ret) {
                responseMsg = std::make_shared<CellularResponseMessage>(true);
                // activate call state timer
                ReloadTimer(callStateTimerId);
                // Propagate "Ringing" notification into system
                sys::Bus::SendMulticast(
                    std::make_shared<CellularCallMessage>(CellularCallMessage::Type::Ringing, msg->number),
                    sys::BusChannels::ServiceCellularNotifications,
                    this);
                break;
            }
        }
        responseMsg = std::make_shared<CellularResponseMessage>(false);
    } break;
    case MessageType::DBServiceNotification: {
        DBNotificationMessage *msg = dynamic_cast<DBNotificationMessage *>(msgl);
        if (msg == nullptr) {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
            break;
        }
        LOG_DEBUG("Received multicast");
        if ((msg->baseType == DB::BaseType::SmsDB) && ((msg->notificationType == DB::NotificationType::Updated) ||
                                                       (msg->notificationType == DB::NotificationType::Added))) {
            sendSMS();
            return std::make_shared<sys::ResponseMessage>();
        }
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Failure);
        break;
    }
    case MessageType::CellularGetIMSI: {

        std::string temp;
        if (getIMSI(temp)) {
            responseMsg = std::make_shared<CellularResponseMessage>(true, temp);
        }
        else {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
        break;
    }
    case MessageType::CellularGetOwnNumber: {
        std::string temp;
        if (getOwnNumber(temp)) {
            responseMsg = std::make_shared<CellularResponseMessage>(true, temp);
        }
        else {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
    } break;
    case MessageType::CellularGetNetworkInfo: {
        LOG_INFO("CellularGetNetworkInfo handled");
        responseMsg = std::make_shared<CellularResponseMessage>(true);

        auto msg  = std::make_shared<cellular::RawCommandRespAsync>(MessageType::CellularNetworkInfoResult);
        msg->data = getNetworkInfo();
        sys::Bus::SendUnicast(msg, msgl->sender, this);
    } break;
    case MessageType::CellularStartOperatorsScan: {
        LOG_INFO("CellularStartOperatorsScan handled");
        cellular::RawCommandResp response = (true);

        auto msg  = std::make_shared<cellular::RawCommandRespAsync>(MessageType::CellularOperatorsScanResult);
        msg->data = scanOperators();
        sys::Bus::SendUnicast(msg, msgl->sender, this);
    } break;
    case MessageType::CellularSelectAntenna: {
        uint8_t value = 0;
        auto msg      = dynamic_cast<CellularRequestMessage *>(msgl);
        if (msg != nullptr) {
            try {
                value = std::stoi(msg->data);
            }
            catch (std::exception &e) {
                LOG_INFO("Service cellular CellularSelectAntenna exception occured: %s", e.what());
            }
            cmux->SelectAntenna(value);
            vTaskDelay(50); // sleep for 50 ms...
            uint8_t actualAntenna = cmux->GetAntenna();
            bool changedAntenna   = (actualAntenna == value);
            responseMsg           = std::make_shared<CellularResponseMessage>(changedAntenna);
        }
        else {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
    } break;
    case MessageType::CellularSetScanMode: {
        auto msg = dynamic_cast<CellularRequestMessage *>(msgl);
        bool ret = false;
        if (msg != nullptr) {
            ret = SetScanMode(msg->data);
        }
        responseMsg = std::make_shared<CellularResponseMessage>(ret);
        break;
    }
    case MessageType::CellularGetScanMode: {

        std::string response;
        response = GetScanMode();

        if (response != "") {
            responseMsg = std::make_shared<CellularResponseMessage>(true);
            auto msg    = std::make_shared<cellular::RawCommandRespAsync>(MessageType::CellularGetScanModeResult);
            msg->data.push_back(response);
            sys::Bus::SendUnicast(msg, msgl->sender, this);
            break;
        }
        responseMsg = std::make_shared<CellularResponseMessage>(false);
        break;
    }
    case MessageType::CellularGetFirmwareVersion: {
        std::string response;
        auto channel = cmux->get(TS0710::Channel::Commands);
        if (channel) {
            auto resp = channel->cmd(at::AT::QGMR);
            if (resp.code == at::Result::Code::OK) {
                response    = resp.response[0];
                responseMsg = std::make_shared<CellularResponseMessage>(true, response);
            }
            else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
        break;
    }
    case MessageType::EVMModemStatus: {
        using namespace bsp::cellular::status;
        auto msg = dynamic_cast<sevm::StatusStateMessage *>(msgl);
        if (msg != nullptr) {
            auto status_pin = msg->state;
            if (status_pin == value::ACTIVE && state.get() == State::ST::PowerUpProcedure && board == bsp::Board::T4) {
                state.set(this, State::ST::PowerUpInProgress); // and go to baud detect as usual
            }
        }
        break;
    }
    default:
        break;

        if (responseMsg == nullptr) {
            LOG_DEBUG("message not handled: %d, %d", static_cast<int>(msgl->type), static_cast<int>(msgl->messageType));
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
    }
    return responseMsg;
}
namespace
{
    bool isAbortCallNotification(const std::string &str)
    {
        return ((str.find(at::Chanel::NO_CARRIER) != std::string::npos) ||
                (str.find(at::Chanel::BUSY) != std::string::npos) ||
                (str.find(at::Chanel::NO_ANSWER) != std::string::npos));
    }
} // namespace

std::optional<std::shared_ptr<CellularMessage>> ServiceCellular::identifyNotification(const std::vector<uint8_t> &data)
{
    std::string str(data.begin(), data.end());

    {
        std::string logStr = utils::removeNewLines(str);
        LOG_DEBUG("Notification:: %s", logStr.c_str());
    }

    if (auto ret = str.find("+CPIN: ") != std::string::npos) {
        /// TODO handle different sim statuses - i.e. no sim, sim error, sim puk, sim pin etc.
        if (str.find("NOT READY", ret) == std::string::npos) {
            if (Store::GSM::get()->selected == Store::GSM::SIM::SIM1) {
                Store::GSM::get()->sim = Store::GSM::SIM::SIM1;
            }
            else {
                Store::GSM::get()->sim = Store::GSM::SIM::SIM2;
            }
            LOG_DEBUG("SIM OK!");
        }
        else {
            LOG_ERROR("SIM ERROR");
            Store::GSM::get()->sim = Store::GSM::SIM::SIM_FAIL;
        }
        if ((str.find("NOT", ret) == std::string::npos) && (str.find("READY", ret) != std::string::npos)) {
            return std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::SIM);
        }
        auto message = std::make_shared<sevm::SIMMessage>();
        sys::Bus::SendUnicast(message, service::name::evt_manager, this);
        //        return std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::SIM);
    }

    // Incoming call
    if (auto ret = str.find("+CLIP: ") != std::string::npos) {
        LOG_TRACE("incoming call...");

        auto beg     = str.find("\"", ret);
        auto end     = str.find("\"", ret + beg + 1);
        auto message = str.substr(beg + 1, end - beg - 1);

        return std::make_shared<CellularCallMessage>(CellularCallMessage::Type::IncomingCall, message);
    }

    // Call aborted/failed
    if (isAbortCallNotification(str)) {
        LOG_TRACE("call aborted");
        return std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallAborted);
    }

    // Received new SMS
    if (str.find("+CMTI: ") != std::string::npos) {
        LOG_TRACE("received new SMS notification");
        // find message number
        auto tokens = utils::split(str, ',');
        if (tokens.size() == 2) {
            return std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::NewIncomingSMS,
                                                                 tokens[1]);
        }
    }

    // Received signal strength change
    auto qind = at::urc::QIND(str);
    if (qind.is() && qind.is_csq()) {
        if (!qind.validate(at::urc::QIND::RSSI)) {
            LOG_INFO("Invalid csq - ignore");
        }
        else {
            SignalStrength signalStrength(std::stoi(qind.tokens[at::urc::QIND::RSSI]));
            if (signalStrength.isValid()) {
                Store::GSM::get()->setSignalStrength(signalStrength.data);
                return std::make_shared<CellularNotificationMessage>(
                    CellularNotificationMessage::Type::SignalStrengthUpdate);
            }
        }
    }

    LOG_WARN("Unhandled notification");
    return std::nullopt;
}

bool ServiceCellular::sendSMS(void)
{
    auto record = DBServiceAPI::SMSGetLastRecord(this);

    // skip if it's not sms to send
    if (record.type != SMSType::QUEUED) {
        return false;
    }

    LOG_INFO("Trying to send SMS");

    uint32_t textLen = record.body.length();

    const uint32_t singleMessageLen = 30;
    bool result                     = false;

    // if text fit in single message send
    if (textLen < singleMessageLen) {

        if (cmux->CheckATCommandPrompt(
                cmux->get(TS0710::Channel::Commands)
                    ->SendCommandPrompt(
                        (std::string(at::factory(at::AT::CMGS)) + UCS2(record.number).modemStr() + "\"\r").c_str(),
                        1,
                        2000))) {

            if (cmux->get(TS0710::Channel::Commands)->cmd((UCS2(record.body).modemStr() + "\032").c_str())) {
                result = true;
            }
            else {
                LOG_INFO("SMS sending failed.");
            }
        }
    }
    // split text, and send concatenated messages
    else {
        const uint32_t maxConcatenatedCount = 7;
        uint32_t messagePartsCount          = textLen / singleMessageLen;
        if ((textLen % singleMessageLen) != 0) {
            messagePartsCount++;
        }

        if (messagePartsCount > maxConcatenatedCount) {
            LOG_ERROR("Message to long");
            return false;
        }

        for (uint32_t i = 0; i < messagePartsCount; i++) {

            uint32_t partLength = singleMessageLen;
            if (i * singleMessageLen + singleMessageLen > record.body.length()) {
                partLength = record.body.length() - i * singleMessageLen;
            }
            UTF8 messagePart = record.body.substr(i * singleMessageLen, partLength);

            std::string command(at::factory(at::AT::QCMGS) + UCS2(record.number).modemStr() + "\",120," +
                                std::to_string(i + 1) + "," + std::to_string(messagePartsCount) + "\r");

            if (cmux->CheckATCommandPrompt(
                    cmux->get(TS0710::Channel::Commands)->SendCommandPrompt(command.c_str(), 1, 5000))) {
                // prompt sign received, send data ended by "Ctrl+Z"
                if (cmux->get(TS0710::Channel::Commands)
                        ->cmd((UCS2(messagePart).modemStr() + "\032").c_str(), 2000, 2)) {
                    result = true;
                }
                else {
                    result = false;
                    LOG_INFO("SMS sending failed.");
                }
            }
        }
    }
    if (result) {
        LOG_INFO("SMS sended.");
        record.type = SMSType::OUTBOX;
        DBServiceAPI::SMSUpdate(this, record);
    }
    return result;
}

bool ServiceCellular::receiveSMS(std::string messageNumber)
{

    auto cmd = at::factory(at::AT::QCMGR);
    auto ret = cmux->get(TS0710::Channel::Commands)->cmd(cmd + messageNumber + "\r", cmd.timeout);

    bool messageParsed = false;

    std::string messageRawBody;
    UTF8 receivedNumber;
    if (ret) {
        for (uint32_t i = 0; i < ret.response.size(); i++) {
            if (ret.response[i].find("QCMGR") != std::string::npos) {

                std::istringstream ss(ret.response[i]);
                std::string token;
                std::vector<std::string> tokens;
                while (std::getline(ss, token, ',')) {
                    tokens.push_back(token);
                }
                tokens[1].erase(std::remove(tokens[1].begin(), tokens[1].end(), '\"'), tokens[1].end());
                /*
                 * tokens:
                 * [0] - +QCMGR
                 * [1] - sender number
                 * [2] - none
                 * [3] - date YY/MM/DD
                 * [4] - hour HH/MM/SS/timezone
                 * concatenaded messages
                 * [5] - unique concatenated message id
                 * [6] - current message number
                 * [7] - total messages count
                 *
                 */
                // parse sender number
                receivedNumber = UCS2(tokens[1]).toUTF8();

                // parse date
                tokens[3].erase(std::remove(tokens[3].begin(), tokens[3].end(), '\"'), tokens[3].end());

                utils::time::Timestamp time;
                time.set_time(tokens[3] + " " + tokens[4], "%y/%m/%d %H:%M:%S");
                auto messageDate = time.getTime();

                // if its single message process
                if (tokens.size() == 5) {

                    messageRawBody = ret.response[i + 1];
                    messageParsed  = true;
                }
                // if its concatenated message wait for last message
                else if (tokens.size() == 8) {

                    uint32_t last    = 0;
                    uint32_t current = 0;
                    try {
                        last    = std::stoi(tokens[7]);
                        current = std::stoi(tokens[6]);
                    }
                    catch (const std::exception &e) {
                        LOG_ERROR("ServiceCellular::receiveSMS error %s", e.what());
                        return false;
                    }
                    if (current == last) {
                        messageParts.push_back(ret.response[i + 1]);

                        for (uint32_t j = 0; j < messageParts.size(); j++) {
                            messageRawBody += messageParts[j];
                        }
                        messageParts.clear();
                        messageParsed = true;
                    }
                    else {
                        messageParts.push_back(ret.response[i + 1]);
                    }
                }
                if (messageParsed) {
                    messageParsed = false;

                    UTF8 decodedMessage = UCS2(messageRawBody).toUTF8();

                    SMSRecord record;
                    record.body   = decodedMessage;
                    record.number = receivedNumber;
                    record.type   = SMSType::INBOX;
                    record.date   = messageDate;

                    DBServiceAPI::SMSAdd(this, record);
                }
            }
        }
    }
    // delete message from modem memory
    cmux->get(TS0710::Channel::Commands)->cmd(at::factory(at::AT::CMGD) + messageNumber);
    return true;
}

bool ServiceCellular::getOwnNumber(std::string &destination)
{
    auto ret = cmux->get(TS0710::Channel::Commands)->cmd(at::AT::CNUM);

    if (ret) {
        auto begin = ret.response[0].find(',');
        auto end   = ret.response[0].rfind(',');
        if (begin != std::string::npos && end != std::string::npos) {
            std::string number;
            try {
                number = ret.response[0].substr(begin, end - begin);
            }
            catch (std::exception &e) {
                LOG_ERROR("ServiceCellular::getOwnNumber exception: %s", e.what());
                return false;
            }
            number.erase(std::remove(number.begin(), number.end(), '"'), number.end());
            number.erase(std::remove(number.begin(), number.end(), ','), number.end());

            destination = number;
            return true;
        }
    }
    LOG_ERROR("ServiceCellular::getOwnNumber failed.");
    return false;
}

bool ServiceCellular::getIMSI(std::string &destination, bool fullNumber)
{
    auto ret = cmux->get(TS0710::Channel::Commands)->cmd(at::AT::CIMI);

    if (ret) {
        if (fullNumber) {
            destination = ret.response[0];
        }
        else {
            try {
                destination = ret.response[0].substr(0, 3);
            }
            catch (std::exception &e) {
                LOG_ERROR("ServiceCellular::getIMSI exception: %s", e.what());
                return false;
            }
        }
        return true;
    }
    LOG_ERROR("ServiceCellular::getIMSI failed.");
    return false;
}
std::vector<std::string> ServiceCellular::getNetworkInfo(void)
{
    std::vector<std::string> data;
    auto channel = cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto resp = channel->cmd(at::AT::CSQ);
        if (resp.code == at::Result::Code::OK) {
            // push back to response message
            std::string ret     = resp.response[0];
            std::string toErase = "+CSQ: ";
            auto pos            = ret.find(toErase);
            if (pos != std::string::npos) {
                ret.erase(pos, toErase.length());
                data.push_back(ret);
            }
            else {
                data.push_back("");
            }
        }
        else {
            data.push_back("");
        }

        resp = channel->cmd(at::AT::CREG);
        if (resp.code == at::Result::Code::OK) {
            std::map<uint32_t, std::string> cregCodes;
            cregCodes.insert(std::pair<uint32_t, std::string>(0, "Not registred"));
            cregCodes.insert(std::pair<uint32_t, std::string>(1, "Registered, home network"));
            cregCodes.insert(std::pair<uint32_t, std::string>(2, "Not registered, searching"));
            cregCodes.insert(std::pair<uint32_t, std::string>(3, "Registration denied"));
            cregCodes.insert(std::pair<uint32_t, std::string>(4, "Unknown"));
            cregCodes.insert(std::pair<uint32_t, std::string>(5, "Registerd, roaming"));

            std::string rawResponse = resp.response[0];
            auto pos                = rawResponse.find(',');

            uint32_t cregValue;
            try {
                cregValue = std::stoi(rawResponse.substr(pos + 1, 1));
            }
            catch (const std::exception &e) {
                LOG_ERROR("ServiceCellular::GetNetworkInfo exception %s", e.what());
                cregValue = UINT32_MAX;
            }
            // push back to response message
            auto commandCode = cregCodes.find(cregValue);
            if (commandCode != cregCodes.end()) {
                data.push_back(commandCode->second);
            }
            else {
                data.push_back("");
            }
        }
        else {
            data.push_back("");
        }

        resp = channel->cmd(at::AT::QNWINFO);
        if (resp.code == at::Result::Code::OK) {
            auto rawResponse = resp.response[0];
            std::string toErase("+QNWINFO: ");
            auto pos = rawResponse.find(toErase);
            if (pos != std::string::npos) {
                rawResponse.erase(pos, toErase.length());
            }
            rawResponse.erase(std::remove(rawResponse.begin(), rawResponse.end(), '\"'), rawResponse.end());
            data.push_back(rawResponse);
        }
        else {
            data.push_back("");
        }
    }
    return data;
}
std::vector<std::string> ServiceCellular::scanOperators(void)
{
    auto channel = cmux->get(TS0710::Channel::Commands);
    std::vector<std::string> result;
    if (channel) {
        auto resp = channel->cmd("AT+COPS=?\r", 180000, 2);
        if (resp.code == at::Result::Code::OK) {
            std::string rawResponse = resp.response[0];
            std::string toErase("+COPS: ");
            auto pos = rawResponse.find(toErase);
            if (pos != std::string::npos) {
                rawResponse.erase(pos, toErase.length());
            }
            std::string delimiter("),(");

            auto end   = rawResponse.find(delimiter);
            auto begin = 0;
            while (end != std::string::npos) {
                auto operatorInfo = rawResponse.substr(begin, end - begin);
                operatorInfo.erase(std::remove(operatorInfo.begin(), operatorInfo.end(), '('), operatorInfo.end());
                operatorInfo.erase(std::remove(operatorInfo.begin(), operatorInfo.end(), ')'), operatorInfo.end());
                operatorInfo.erase(std::remove(operatorInfo.begin(), operatorInfo.end(), ','), operatorInfo.end());
                result.push_back(operatorInfo);
                begin = end;
                end   = rawResponse.find(delimiter, end + 1);
            }
        }
    }
    return result;
}

std::vector<std::string> get_last_AT_error(DLC_channel *channel)
{
    auto ret = channel->cmd(at::AT::CEER);
    return std::move(ret.response);
}

void log_last_AT_error(DLC_channel *channel)
{
    std::vector<std::string> atErrors(get_last_AT_error(channel));
    int i = 1;
    for (auto &msg_line : atErrors) {
        LOG_ERROR("%d/%d: %s", i, static_cast<int>(atErrors.size()), msg_line.c_str());
        i++;
    }
}

bool is_SIM_detection_enabled(DLC_channel *channel)
{
    auto ret = channel->cmd(at::AT::SIM_DET);
    if (ret) {
        if (ret.response[0].find("+QSIMDET: 1") != std::string::npos) {
            LOG_DEBUG("SIM detecition enabled!");
            return true;
        }
    }
    else {
        LOG_FATAL("Cant check sim detection status!");
        log_last_AT_error(channel);
    }
    return false;
}

bool enable_SIM_detection(DLC_channel *channel)
{
    auto ret = channel->cmd(at::AT::SIM_DET_ON);
    if (!ret) {
        log_last_AT_error(channel);
        return false;
    }
    return true;
}

bool is_SIM_status_enabled(DLC_channel *channel)
{
    auto ret = channel->cmd(at::AT::QSIMSTAT);
    if (ret) {
        if (ret.response[0].find("+QSIMSTAT: 1") != std::string::npos) {
            LOG_DEBUG("SIM swap enabled!");
            return true;
        }
    }
    else {
        LOG_FATAL("SIM swap status failure! %s", ret.response[0].c_str());
        log_last_AT_error(channel);
    }
    return false;
}

bool enable_SIM_status(DLC_channel *channel)
{
    auto ret = channel->cmd(at::AT::SIMSTAT_ON);
    if (!ret) {
        log_last_AT_error(channel);
        return false;
    }
    return true;
}

void save_SIM_detection_status(DLC_channel *channel)
{
    auto ret = channel->cmd(at::AT::STORE_SETTINGS_ATW);
    if (!ret) {
        log_last_AT_error(channel);
    }
}

bool sim_check_hot_swap(DLC_channel *channel)
{
    assert(channel);
    bool reboot_needed = false;

    if (!is_SIM_detection_enabled(channel)) {
        reboot_needed = true;
    }
    if (!is_SIM_status_enabled(channel)) {
        reboot_needed = true;
    }
    if (reboot_needed) {
        enable_SIM_detection(channel);
        enable_SIM_status(channel);
        save_SIM_detection_status(channel);
        LOG_FATAL("Modem reboot required, Please remove battery!");
    }
    return !reboot_needed;
}

bool ServiceCellular::handle_sim_sanity_check()
{
    auto ret = sim_check_hot_swap(cmux->get(TS0710::Channel::Commands));
    if (ret) {
        state.set(this, State::ST::ModemOn);
    }
    else {
        LOG_ERROR("Sanity check failure - user will be promped about full shutdown");
        state.set(this, State::ST::ModemFatalFailure);
    }
    return ret;
}

bool ServiceCellular::handle_select_sim()
{

    bsp::cellular::sim::sim_sel();
    bsp::cellular::sim::hotswap_trigger();
#if defined(TARGET_Linux)
    auto ret = cmux->get(TS0710::Channel::Commands)->cmd(at::AT::QSIMSTAT);
    if (!ret) {
        LOG_FATAL("Cant check sim stat status");
    }
    else {
        if (ret.response[0].find("+QSIMSTAT: 1,1") != std::string::npos) {
            // SIM IN - only sim1 mocup
            Store::GSM::get()->sim = Store::GSM::SIM::SIM1;
        }
        else {
            // NO SIM IN
            Store::GSM::get()->sim = Store::GSM::SIM::SIM_FAIL;
        }
        sys::Bus::SendUnicast(std::make_shared<sevm::SIMMessage>(), service::name::evt_manager, this);
    }
#endif
    return true;
}

bool ServiceCellular::handle_modem_on()
{
    state.set(this, State::ST::Idle);
    auto channel = cmux->get(TS0710::Channel::Commands);
    channel->cmd("AT+CCLK?\r");
    // inform host ap ready
    cmux->InformModemHostWakeup();
    LOG_DEBUG("AP ready");
    return true;
}

bool ServiceCellular::handle_sim_init()
{
    auto channel = cmux->get(TS0710::Channel::Commands);
    if (channel == nullptr) {
        LOG_ERROR("Cant configure sim! no Commands channel!");
        state.set(this, State::ST::Failed);
        return false;
    }
    bool success  = true;
    auto commands = at::getCommadsSet(at::commadsSet::simInit);

    for (auto command : commands) {
        if (!channel->cmd(command)) {
            LOG_ERROR("SIM initialization failure!");
            return false;
        }
    }

    state.set(this, State::ST::Idle);
    return success;
}

bool ServiceCellular::handle_failure()
{
    state.set(this, State::ST::Idle);
    return true;
}

bool ServiceCellular::handle_fatal_failure()
{
    LOG_FATAL("Await for death!");
    while (true) {
        vTaskDelay(500);
    }
    return true;
}

bool ServiceCellular::SetScanMode(std::string mode)
{
    auto channel = cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto command = at::factory(at::AT::SET_SCANMODE);

        auto resp = channel->cmd(command.cmd + mode + ",1\r", 300, 1);
        if (resp.code == at::Result::Code::OK) {
            return true;
        }
    }
    return false;
}

std::string ServiceCellular::GetScanMode(void)
{
    auto channel = cmux->get(TS0710::Channel::Commands);
    if (channel) {

        auto resp = channel->cmd(at::AT::GET_SCANMODE);
        if (resp.code == at::Result::Code::OK) {
            auto beg = resp.response[0].find(",");
            if (beg != std::string::npos) {
                auto response = resp.response[0].substr(beg + 1, 1);
                return response;
            }
        }
    }
    return ("");
}

bool ServiceCellular::handle_status_check(void)
{
    LOG_INFO("Checking modem status.");
    auto modemActive = cmux->IsModemActive();
    if (modemActive) {
        // modem is already turned on, call configutarion procedure
        LOG_INFO("Modem is already turned on.");
        LOG_DEBUG("T4 - hot start");
        state.set(this, cellular::State::ST::PowerUpInProgress);
    }
    else {
        state.set(this, cellular::State::ST::PowerUpProcedure);
    }
    return true;
}

void ServiceCellular::startStateTimer(uint32_t timeout)
{
    stateTimeout = timeout;
    ReloadTimer(stateTimerId);
}

void ServiceCellular::stopStateTimer()
{
    stateTimeout = 0;
    stopTimer(stateTimerId);
}

void ServiceCellular::handleStateTimer(void)
{
    stateTimeout--;
    if (stateTimeout == 0) {
        stopStateTimer();
        LOG_FATAL("State %s timeout occured!", state.c_str(state.get()));
        state.set(this, cellular::State::ST::ModemFatalFailure);
    }
}
