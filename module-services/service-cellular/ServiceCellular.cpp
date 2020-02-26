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

#include "service-evtmgr/messages/EVMessages.hpp"
#include "ucs2/UCS2.hpp"

#include "service-db/api/DBServiceAPI.hpp"
#include "service-db/messages/DBNotificationMessage.hpp"

#include "time/time_conversion.hpp"
#include <Utils.hpp>
#include <common_data/EventStore.hpp>

const char *ServiceCellular::serviceName = "ServiceCellular";
constexpr int32_t ServiceCellular::signalStrengthToDB[];

inline const auto cellularStack = 16384UL;

ServiceCellular::ServiceCellular() : sys::Service(serviceName, "", cellularStack, sys::ServicePriority::Idle)
{

    LOG_INFO("[ServiceCellular] Initializing");

    busChannels.push_back(sys::BusChannels::ServiceCellularNotifications);
    busChannels.push_back(sys::BusChannels::ServiceDBNotifications);

    callStateTimerId = CreateTimer(Ticks::MsToTicks(1000), true);

    ongoingCall.setStartCallAction([=](const CalllogRecord &rec) {
        uint32_t callId = DBServiceAPI::CalllogAdd(this, rec);
        if (callId == 0)
        {
            LOG_ERROR("CalllogAdd failed");
        }
        return callId;
    });

    ongoingCall.setEndCallAction([=](const CalllogRecord &rec) { return DBServiceAPI::CalllogUpdate(this, rec); });

    notificationCallback = [this](std::vector<uint8_t> &data) {
        LOG_DEBUG("Notifications callback called with %i data bytes", data.size());
        TS0710_Frame frame(data);
        std::string message;
        CellularNotificationMessage::Type type = identifyNotification(frame.getFrame().data, message);
        auto msg = std::make_shared<CellularNotificationMessage>(type);

        switch (type)
        {

        case CellularNotificationMessage::Type::PowerUpProcedureComplete:
        case CellularNotificationMessage::Type::Ringing:
        case CellularNotificationMessage::Type::ServiceReady:
        case CellularNotificationMessage::Type::CallBusy:
        case CellularNotificationMessage::Type::CallActive:
        case CellularNotificationMessage::Type::CallAborted:
            // no data field is used
            break;

        case CellularNotificationMessage::Type::IncomingCall:
            msg->data = message;
            break;

        case CellularNotificationMessage::Type::NewIncomingSMS: {
            // find message number
            std::string notification(data.begin(), data.end());
            auto begin = notification.find(",");
            auto end = notification.find("\r");
            msg->data = notification.substr(begin + 1, end);
        }
        break;

        case CellularNotificationMessage::Type::SignalStrengthUpdate:
            LOG_DEBUG("Setting new signal strength");
            msg->signalStrength = std::stoll(message);
            if (msg->signalStrength > (sizeof(signalStrengthToDB) / sizeof(signalStrengthToDB[0])))
            {
                LOG_ERROR("Signal strength value out of range.");
                msg->dBmSignalStrength = signalStrengthToDB[0];
            }
            else
            {
                msg->dBmSignalStrength = signalStrengthToDB[msg->signalStrength];
            }

            break;

            case CellularNotificationMessage::Type::None:
                // do not send notification msg
                return;
            case CellularNotificationMessage::Type::RawCommand: {
                LOG_INFO(" IGNORE RawCmd");
                return;
            }
        }

        sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceCellularNotifications, this);
    };
}

ServiceCellular::~ServiceCellular() {

    LOG_INFO("[ServiceCellular] Cleaning resources");
    if (cmux != nullptr) {
        delete cmux;
    }
}

void ServiceCellular::CallStateTimerHandler()
{
    std::shared_ptr<CellularRequestMessage> msg = std::make_shared<CellularRequestMessage>(MessageType::CellularListCurrentCalls);
    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, this);
}

// Invoked when timer ticked
void ServiceCellular::TickHandler(uint32_t id)
{
    if (id == callStateTimerId)
    {
        CallStateTimerHandler();
    }
    else
    {
        LOG_ERROR("Unrecognized timer ID = %u", id);
    }
}

// Invoked during initialization
sys::ReturnCodes ServiceCellular::InitHandler() {

    //cmux = new TS0710(PortSpeed_e::PS460800, this);

    // Start procedure is as follow:
        /*
         * 1) Power-up
         * 2) Init configuration of GSM modem
         * 3) Audio configuration
         * 4) Start multiplexer
         * 5) Modem fully-operational
         */

    // Start power-up procedure
    sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartPowerUpProcedure),
                          GetName(), this);
    state = State::PowerUpInProgress;

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceCellular::DeinitHandler() {

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceCellular::SwitchPowerModeHandler(const sys::ServicePowerMode mode) {
    LOG_FATAL("[ServiceCellular] PowerModeHandler: %d", static_cast<uint32_t>(mode));

    switch (mode){
        case sys::ServicePowerMode ::Active:
            //muxdaemon->ExitSleepMode();
            break;
        case sys::ServicePowerMode ::SuspendToRAM:
        case sys::ServicePowerMode ::SuspendToNVM:
        	LOG_FATAL("TEMPORARY DISABLED!!!!! UNCOMMENT WHEN READY.");
//            muxdaemon->EnterSleepMode();
            break;
    }

    return sys::ReturnCodes::Success;
}

sys::Message_t ServiceCellular::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    std::shared_ptr<sys::ResponseMessage> responseMsg;

    switch (static_cast<MessageType>(msgl->messageType))
    {
    // Incoming notifications from Notification Virtual Channel
    case MessageType::CellularNotification: {
        CellularNotificationMessage *msg = dynamic_cast<CellularNotificationMessage *>(msgl);
        if (msg != nullptr)
        {
            switch (msg->type)
            {
            case CellularNotificationMessage::Type::CallActive: {
                auto ret = ongoingCall.setActive();
                responseMsg = std::make_shared<CellularResponseMessage>(ret);
                break;
            }
            case CellularNotificationMessage::Type::IncomingCall: {
                auto ret = true;
                if (!ongoingCall.isValid())
                {
                    // CellularNotificationMessage::Type::IncomingCall is called periodically during not answered incomming call
                    // create ongoing call only once
                    ret = ongoingCall.startCall(msg->data, CallType::CT_INCOMING);
                }
                responseMsg = std::make_shared<CellularResponseMessage>(ret);
                break;
            }
            case CellularNotificationMessage::Type::CallAborted:
            case CellularNotificationMessage::Type::CallBusy: {
                stopTimer(callStateTimerId);
                auto ret = ongoingCall.endCall();
                responseMsg = std::make_shared<CellularResponseMessage>(ret);
                break;
            }
            case CellularNotificationMessage::Type::Ringing: {
                auto ret = ongoingCall.startCall(msg->data, CallType::CT_OUTGOING);
                responseMsg = std::make_shared<CellularResponseMessage>(ret);
                break;
            }
            case CellularNotificationMessage::Type::PowerUpProcedureComplete: {
                sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartConfProcedure), GetName(), this);
                state = State ::ModemConfigurationInProgress;
                break;
            }
            case CellularNotificationMessage::Type::NewIncomingSMS: {
                LOG_INFO("New incoming sms received");
                receiveSMS(msg->data);
                break;
            }
            case CellularNotificationMessage::Type::RawCommand: {
                auto m = dynamic_cast<cellular::RawCommand *>(msgl);
                auto channel = cmux->GetChannel("Commands");
                if (!m || !channel)
                {
                    LOG_ERROR("RawCommand error: %s %s", m == nullptr ? "" : "bad cmd", !channel ? "no channel" : "");
                    break;
                }
                auto respMsg = std::make_shared<cellular::RawCommandResp>(true);
                auto ret = channel->cmd(m->command.c_str(), m->timeout);
                respMsg->response = ret.response;
                if (respMsg->response.size())
                {
                    for (auto const &el : respMsg->response)
                    {
                        LOG_DEBUG("> %s", el.c_str());
                    }
                }
                responseMsg = respMsg;
                break;
            }
            break;
            default: {
                LOG_INFO("Skipped CellularNotificationMessage::Type %d", msg->type);
            }
            }
        }
    }
    break;

    case MessageType::CellularStartPowerUpProcedure: {
        auto powerRet = cmux->PowerUpProcedure();
        if (powerRet == TS0710::ConfState::Success)
        {
            sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartConfProcedure), GetName(), this);
        }
        else if (powerRet == TS0710::ConfState::PowerUp)
        {
            state = State::PowerUpInProgress;
        }
        else
        {
            LOG_FATAL("[ServiceCellular] PowerUp procedure failed");
            state = State::Failed;
        }
    }
    break;

    case MessageType::CellularStartConfProcedure: {
        // Start configuration procedure, if it's first run modem will be restarted
        auto confRet = cmux->ConfProcedure();
        if (confRet == TS0710::ConfState::Success)
        {
            sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartAudioConfProcedure), GetName(), this);
            state = State::AudioConfigurationInProgress;
        }
        else
        {
            LOG_FATAL("[ServiceCellular] Initialization failed, not ready");
            state = State::Failed;
        }
    }
    break;

    case MessageType ::CellularStartAudioConfProcedure: {
        auto audioRet = cmux->AudioConfProcedure();
        if (audioRet == TS0710::ConfState::Success)
        {
            auto cmd = at::factory(at::AT::IPR) + std::to_string(ATPortSpeeds_text[cmux->getStartParams().PortSpeed]) + "\r";
            LOG_DEBUG("Setting baudrate %i baud", ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
            if (!cmux->getParser()->cmd(cmd))
            {
                LOG_ERROR("Baudrate setup error");
                state = State::Failed;
                break;
            }
            cmux->getCellular()->SetSpeed(ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);

            vTaskDelay(1000);

            if (cmux->StartMultiplexer() == TS0710::ConfState::Success)
            {

                LOG_DEBUG("[ServiceCellular] Modem is fully operational");

                DLC_channel *notificationsChannel = cmux->GetChannel("Notifications"); // open channel - notifications
                if (notificationsChannel)
                {
                    LOG_DEBUG("Setting up notifications callback");
                    notificationsChannel->setCallback(notificationCallback);
                }

                state = State::Ready;

            }
            else
            {
                LOG_DEBUG("[ServiceCellular] Modem FAILED");
                state = State::Failed;
            }

            state = State::Ready;
            // Propagate "ServiceReady" notification into system
            sys::Bus::SendMulticast(std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::ServiceReady),
                                    sys::BusChannels::ServiceCellularNotifications, this);
        }
        else if (audioRet == TS0710::ConfState::Failure)
        {
            sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartAudioConfProcedure), GetName(), this);
        }
        else
        {
            // Reset procedure started, do nothing here
            state = State::PowerUpInProgress;
        }
    }
    break;

    case MessageType::CellularListCurrentCalls: {
        constexpr size_t numberOfExpectedTokens = 3;
        auto ret = cmux->GetChannel("Commands")->cmd(at::AT::CLCC);
        if (ret && ret.response.size() == numberOfExpectedTokens)
        {
            // TODO: alek: add case when more status calls is returned
            // TODO: alek: add cellular call validation and check it with modemcall

            // TODO: alek - just handle parts of response properly
            // if CellularListCurrentCalls is recieved after the call is aborted it will return 2 tokens instead of 3
            // this should be acceptable and hence warning instead of error is logged in such case
            bool retVal = true;
            auto callEntry = ret.response[1];

            try
            {
                ModemCall::ModemCall call(callEntry);
                LOG_DEBUG("%s", utils::to_string(call).c_str());
                // If call changed to "Active" state stop callStateTimer(used for polling for call state)
                if (call.state == ModemCall::CallState::Active)
                {
                    auto msg = std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallActive);
                    sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceCellularNotifications, this);
                    stopTimer(callStateTimerId);
                }
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("exception \"%s\" was thrown", e.what());
                assert(0);
                retVal = false;
            }
            responseMsg = std::make_shared<CellularResponseMessage>(retVal);
        }
        else
        {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
    }
    break;

    case MessageType::CellularHangupCall: {
        auto channel = cmux->GetChannel("Commands");
        LOG_INFO("CellularHangupCall");
        if (channel)
        {
            if (channel->cmd(at::AT::ATH))
            {
                responseMsg = std::make_shared<CellularResponseMessage>(true);
                stopTimer(callStateTimerId);
                // Propagate "CallAborted" notification into system
                sys::Bus::SendMulticast(std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallAborted),
                                        sys::BusChannels::ServiceCellularNotifications, this);
            }
            else
            {
                LOG_ERROR("Call not aborted");
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
           break;
        }
        responseMsg = std::make_shared<CellularResponseMessage>(false);
    }
    break;

    case MessageType::CellularAnswerIncomingCall: {
        auto channel = cmux->GetChannel("Commands");
        auto ret = false;
        if (channel)
        {
            // TODO alek: check if your request isn't for 5 sec when you wait in command for 90000, it's exclusivelly set to 5000ms in command requesting...
            auto response = channel->cmd(at::AT::ATA);
            if (response)
            {
                // Propagate "CallActive" notification into system
                sys::Bus::SendMulticast(std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallActive),
                                        sys::BusChannels::ServiceCellularNotifications, this);
                ret = true;
            }
        }
        responseMsg = std::make_shared<CellularResponseMessage>(ret);
    }
    break;

	case MessageType::CellularDialNumber: {
        CellularRequestMessage *msg = reinterpret_cast<CellularRequestMessage *>(msgl);
        auto channel = cmux->GetChannel("Commands");
		if (channel) {
            auto ret = channel->cmd(at::factory(at::AT::ATD) + msg->data + ";\r");
            if (ret)
            {
                responseMsg = std::make_shared<CellularResponseMessage>(true);
                // activate call state timer
                ReloadTimer(callStateTimerId);
                // Propagate "Ringing" notification into system
                sys::Bus::SendMulticast(std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::Ringing, msg->data),
                                        sys::BusChannels::ServiceCellularNotifications, this);
                break;
            }
        }
        responseMsg = std::make_shared<CellularResponseMessage>(false);
	}
		break;
        case MessageType::DBServiceNotification: {
            DBNotificationMessage *msg = dynamic_cast<DBNotificationMessage *>(msgl);
            if (msg == nullptr)
            {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
                break;
            }
            LOG_DEBUG("Received multicast");
            if ((msg->baseType == DB::BaseType::SmsDB) &&
                ((msg->notificationType == DB::NotificationType::Updated) || (msg->notificationType == DB::NotificationType::Added)))
            {
                sendSMS();
                return std::make_shared<sys::ResponseMessage>();
            }
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Failure);
        break;
	}
    case MessageType::CellularGetIMSI: {

        std::string temp;
        if (getIMSI(temp))
        {
            responseMsg = std::make_shared<CellularResponseMessage>(true, temp);
        }
        else
        {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
        break;
    }
    case MessageType::CellularGetOwnNumber: {
        std::string temp;
        if (getOwnNumber(temp))
        {
            responseMsg = std::make_shared<CellularResponseMessage>(true, temp);
        }
        else
        {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
    }
    break;
    case MessageType::CellularGetNetworkInfo: {
        LOG_INFO("CellularGetNetworkInfo handled");
        responseMsg = std::make_shared<CellularResponseMessage>(true);

        auto msg = std::make_shared<cellular::RawCommandRespAsync>(MessageType::CellularNetworkInfoResult);
        msg->data = getNetworkInfo();
        sys::Bus::SendUnicast(msg, msgl->sender, this);
    }
    break;
    case MessageType::CellularStartOperatorsScan: {
        LOG_INFO("CellularStartOperatorsScan handled");
        cellular::RawCommandResp response = (true);

        auto msg = std::make_shared<cellular::RawCommandRespAsync>(MessageType::CellularOperatorsScanResult);
        msg->data = scanOperators();
        sys::Bus::SendUnicast(msg, msgl->sender, this);
    }
    break;
    case MessageType::CellularSelectAntenna: {
        uint8_t value = 0;
        auto msg = dynamic_cast<CellularRequestMessage *>(msgl);
        if (msg != nullptr)
        {
            try
            {
                value = std::stoi(msg->data);
            }
            catch (std::exception &e)
            {
                LOG_INFO("Service cellular CellularSelectAntenna exception occured: %s", e.what());
            }
            cmux->SelectAntenna(value);
            responseMsg = std::make_shared<CellularResponseMessage>(true);
        }
        else
        {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
    }
    break;
    default:
        break;
    }

    if (responseMsg == nullptr)
    {
        LOG_DEBUG("message not handled: %d, %d", msgl->type, msgl->messageType);
        responseMsg = std::make_shared<CellularResponseMessage>(false);
    }

    return responseMsg;
}


CellularNotificationMessage::Type ServiceCellular::identifyNotification(std::vector<uint8_t> data, std::string &message) {

    /* let's convert uint8_t vector to std::string*/
    std::string str = std::string(reinterpret_cast<char*>(data.data()), reinterpret_cast<char*>(data.data() + data.size()));

    if (auto ret = str.find("+CPIN: ") != std::string::npos)
    {
        /// TODO handle different sim statuses - i.e. no sim, sim error, sim puk, sim pin etc.
        if (str.find("NOT READY", ret) == std::string::npos)
        {
            if (Store::GSM::get()->selected == Store::GSM::SIM::SIM1)
            {
                Store::GSM::get()->sim = Store::GSM::SIM::SIM1;
            }
            else
            {
                Store::GSM::get()->sim = Store::GSM::SIM::SIM2;
            }
            LOG_DEBUG("SIM OK!");
        }
        else
        {
            LOG_ERROR("SIM ERROR");
            Store::GSM::get()->sim = Store::GSM::SIM::NONE;
        }
        auto message = std::make_shared<sevm::SIMMessage>();
        sys::Bus::SendUnicast(message, "EventManager", this);
    }

    // Incoming call
    if (auto ret = str.find("+CLIP: ") != std::string::npos) {
        LOG_TRACE("incoming call...");

        auto beg = str.find("\"",ret);
        auto end = str.find("\"",ret + beg+1);
        message = str.substr(beg+1,end-beg-1);

        return CellularNotificationMessage::Type::IncomingCall;
    }


    // Call aborted/failed
    if (str.find("NO CARRIER") != std::string::npos) {
        LOG_TRACE(": call failed/aborted");
        return CellularNotificationMessage::Type::CallAborted;
    }

    // Call busy
    if (str.find("BUSY") != std::string::npos) {
        LOG_TRACE(": call busy");
        return CellularNotificationMessage::Type::CallBusy;
    }

    // Received new SMS
    if (str.find("+CMTI: ") != std::string::npos) {
        LOG_TRACE(": received new SMS notification");
        message = "888777333"; // TODO:M.P add SMS nr parsing

        return CellularNotificationMessage::Type::NewIncomingSMS;
    }

    // Received signal strength change
    if (auto ret = str.find("+QIND: \"csq\"") != std::string::npos) {
        LOG_TRACE(": received signal strength change notification");
        auto beg = str.find(",",ret);
        auto end = str.find(",",ret + beg+1);
        message = str.substr(beg+1,end-beg-1);

        return CellularNotificationMessage::Type::SignalStrengthUpdate;
    }

    LOG_TRACE(": None");
    return CellularNotificationMessage::Type::None;

}
