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

#include "Common.hpp"
#include "log/log.hpp"

#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "ServiceCellular.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

#include "MessageType.hpp"

#include "messages/CellularMessage.hpp"
#include <ticks.hpp>

#include "CommonCellular.hpp"
#include "log/log.hpp"

#include "ucs2/UCS2.hpp"

#include "service-db/api/DBServiceAPI.hpp"
#include "service-db/messages/DBNotificationMessage.hpp"

#include "time/time_conversion.hpp"

const char *ServiceCellular::serviceName = "ServiceCellular";
constexpr int32_t ServiceCellular::signalStrengthToDB[];


ServiceCellular::ServiceCellular()
        : sys::Service(serviceName, "", 8192UL, sys::ServicePriority::Idle) {

    LOG_INFO("[ServiceCellular] Initializing");

    busChannels.push_back(sys::BusChannels::ServiceCellularNotifications);
    busChannels.push_back(sys::BusChannels::ServiceDBNotifications);

    callStateTimerId = CreateTimer(Ticks::MsToTicks(1000), true);
    callDurationTimerId = CreateTimer(Ticks::MsToTicks(1000), true);

    notificationCallback = [this](std::vector<uint8_t> &data) {
        LOG_DEBUG("Notifications callback called with %i data bytes", data.size());
        TS0710_Frame frame(data);
        std::string message;
        CellularNotificationMessage::Type type = identifyNotification(frame.getFrame().data, message);
        auto msg = std::make_shared<CellularNotificationMessage>(type);

        switch (type) {

            case CellularNotificationMessage::Type::PowerUpProcedureComplete: {
                sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartConfProcedure),
                                      GetName(), this);
                state = State ::ModemConfigurationInProgress;
                return;
            }
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

            case CellularNotificationMessage::Type::NewIncomingSMS:
            {
            	//find message number
            	std::string notification(data.begin(), data.end() );
            	auto begin = notification.find(",");
            	auto end = notification.find("\r");
            	msg->data  = notification.substr(begin + 1, end);
            }
                break;

            case CellularNotificationMessage::Type::SignalStrengthUpdate:
                LOG_DEBUG("Setting new signal strength");
                msg->signalStrength = std::stoll(message);
                if (msg->signalStrength > (sizeof(signalStrengthToDB) / sizeof(signalStrengthToDB[0]))) {
                    LOG_ERROR("Signal strength value out of range.");
                    msg->dBmSignalStrength = signalStrengthToDB[0];
                } else {
                    msg->dBmSignalStrength = signalStrengthToDB[msg->signalStrength];
                }

                break;
            
            case CellularNotificationMessage::Type::None:
                // do not send notification msg
                return;
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

void ServiceCellular::CallDurationTimerHandler()
{
    callDuration++;
}

// Invoked when timer ticked
void ServiceCellular::TickHandler(uint32_t id)
{
    if (id == callStateTimerId)
    {
        CallStateTimerHandler();
    }
    else if (id == callDurationTimerId)
    {
        CallDurationTimerHandler();
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
                runCallDurationTimer();
                break;
            }
            case CellularNotificationMessage::Type::CallAborted:
            case CellularNotificationMessage::Type::CallBusy: {
                stopTimer(callDurationTimerId);
                stopTimer(callStateTimerId);
                // TODO: alek: clean this up
                // ************************** This is only temporary to check DB api is working as expected **********
                LOG_DEBUG("callDuration %d, callEndTime %d", callDuration, callEndTime);
                CalllogRecord calllogRec;
                calllogRec.number = "+48600226908";
                calllogRec.presentation = PresentationType::PR_ALLOWED;
                time_t timestamp;
                bsp::rtc_GetCurrentTimestamp(&timestamp);
                calllogRec.date = timestamp;
                calllogRec.duration = callDuration;
                calllogRec.type = CallType::CT_OUTGOING;
                calllogRec.name = "test name";
                calllogRec.contactId = "1";

                if (!DBServiceAPI::CalllogAdd(this, calllogRec))
                {
                    LOG_ERROR("CalllogAdd failed");
                }
                // *****************************************************************************************************
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
            default: {
                // ignore rest of notifications
            }
            }
        }
        else if (msg->type == CellularNotificationMessage::Type::PowerUpProcedureComplete)
		{
			sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartConfProcedure),
								  GetName(), this);
			state = State ::ModemConfigurationInProgress;
		}
		else if(msg->type == CellularNotificationMessage::Type::NewIncomingSMS)
		{
			LOG_INFO("New incoming sms received");
			receiveSMS(msg->data);
		}
        else
        {
            LOG_ERROR("Not CellularNotificationMessage");
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
            std::string buf = "AT+IPR=" + std::to_string(ATPortSpeeds_text[cmux->getStartParams().PortSpeed]) + "\r";
            LOG_DEBUG("Setting baudrate %i baud", ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
            if (!cmux->CheckATCommandResponse(cmux->getParser()->SendCommand(buf.c_str(), 1)))
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
        auto ret = cmux->GetChannel("Commands")->SendCommandResponse("AT+CLCC\r", numberOfExpectedTokens, 300);
        // if CellularListCurrentCalls is recieved after the call is aborted it will return 2 tokens instead of 3
        // this should be acceptable and hence warning instead of error is logged in such case
        if (cmux->CheckATCommandResponse(ret, numberOfExpectedTokens, LOGWARN))
        {
            auto beg = ret[1].find(",", 0);
            beg = ret[1].find(",", beg + 1);
            try
            {
                // If call changed to "Active" state stop callStateTimer(used for polling for call state)
                if (std::stoul(ret[1].substr(beg + 1, 1)) == static_cast<uint32_t>(CallStates::Active))
                {
                    auto msg = std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallActive);
                    sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceCellularNotifications, this);
                    stopTimer(callStateTimerId);
                }
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("exception %s was thrown", e.what());
                assert(0);
            }

            responseMsg = std::make_shared<CellularResponseMessage>(true);
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
            if (cmux->CheckATCommandResponse(channel->SendCommandResponse("ATH\r", 1, 5000)))
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
        if (channel)
        {
            // per Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf timeout should be possibly set up to 90s
            auto ret = channel->SendCommandResponse("ATA\r", 1, 90000);
            if (cmux->CheckATCommandResponse(ret))
            {
                responseMsg = std::make_shared<CellularResponseMessage>(true);
                // Propagate "CallActive" notification into system
                sys::Bus::SendMulticast(std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallActive),
                                        sys::BusChannels::ServiceCellularNotifications, this);
                break;
            }
        }
        responseMsg = std::make_shared<CellularResponseMessage>(false);
    }
    break;

	case MessageType::CellularDialNumber: {
		CellularRequestMessage *msg =
				reinterpret_cast<CellularRequestMessage*>(msgl);
		auto channel = cmux->GetChannel("Commands");
		if (channel) {
			auto ret = channel->SendCommandResponse(
					("ATD" + msg->data + ";\r").c_str(), 1, 5000);
			if (cmux->CheckATCommandResponse(ret)) {
				responseMsg = std::make_shared<CellularResponseMessage>(true);
				// activate call state timer
                ReloadTimer(callStateTimerId);
                // Propagate "Ringing" notification into system
				sys::Bus::SendMulticast(
						std::make_shared<CellularNotificationMessage>(
								CellularNotificationMessage::Type::Ringing,
								msg->data),
						sys::BusChannels::ServiceCellularNotifications, this);
				break;
			}
		}
		responseMsg = std::make_shared<CellularResponseMessage>(false);
	}
		break;
	case MessageType::DBServiceNotification :
	{
		DBNotificationMessage *msg = reinterpret_cast<DBNotificationMessage*>(msgl);

		LOG_DEBUG("Received multicast");
		if((msg->baseType == DBBaseType::SmsDB) && (msg->notificationType == DBNotificatonType::Updated))
		{
			sendSMS();
		}

		break;
	}
    default:
        break;
    }

    return responseMsg;
}

CellularNotificationMessage::Type ServiceCellular::identifyNotification(std::vector<uint8_t> data, std::string &message) {

    /* let's convert uint8_t vector to std::string*/
    std::string str = std::string(reinterpret_cast<char*>(data.data()), reinterpret_cast<char*>(data.data() + data.size()));

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

bool ServiceCellular::sendSMS(void)
{
	auto record = DBServiceAPI::SMSGetLastRecord(this);

	// skip if it's not sms to send
	if(record.type != SMSType::QUEUED)
	{
		return false;
	}

	LOG_INFO("Trying to send SMS");

	uint32_t textLen = record.body.length();

	const uint32_t singleMessageLen = 30;
	bool result = false;

	//if text fit in single message send
	if (textLen < singleMessageLen) {

		auto retCommand = cmux->GetChannel("Commands")->SendCommandPrompt(
				("AT+CMGS=\"" + UCS2(record.number).modemStr() + "\"\r").c_str(), 1, 1000);

		if ((retCommand.size() == 1) && (retCommand[0] == ">")) {
			auto retText = cmux->GetChannel("Commands")->SendCommandResponse(
					(UCS2(record.body).modemStr() + "\032").c_str(), 1);
			//check modem response
			record.type = SMSType::OUTBOX;

			result = true;
		}
	}
	//split text, and send concatenated messages
	else {
		const uint32_t maxConcatenatedCount = 7;
		uint32_t messagePartsCount = textLen / singleMessageLen;
		if( (textLen % singleMessageLen) != 0 )
		{
			messagePartsCount++;
		}

		if(messagePartsCount > maxConcatenatedCount)
		{
			LOG_ERROR("Message to long");
			return false;
		}

		for(uint32_t i = 0; i < messagePartsCount; i++)
		{

			uint32_t partLength = singleMessageLen;
			if (i * singleMessageLen + singleMessageLen > record.body.length()) {
				partLength = record.body.length() - i * singleMessageLen;
			}
			std::string messagePart = record.body.substr(i * singleMessageLen,
					partLength);


			std::string command(
					"AT+QCMGS=\"" + UCS2(record.number).modemStr() + "\",120,"
							+ std::to_string(i + 1) + ","
							+ std::to_string(messagePartsCount) + "\r");

			auto retCommand = cmux->GetChannel("Commands")->SendCommandPrompt(
					command.c_str(), 1, 1000);

			if ((retCommand.size() == 1) && (retCommand[0] == ">")) {
				//prompt sign received, send data ended by "Ctrl+Z"
				auto sended =
						cmux->GetChannel("Commands")->SendCommandResponse(
								 (UCS2(messagePart).modemStr()
										+ "\032").c_str(), 2, 2000);
				//check modem response
				record.type = SMSType::OUTBOX;
			}
		}
		result = true;
	}
    if (result)
    {
        DBServiceAPI::SMSUpdate(this, record);
    }
    return result;
}

bool ServiceCellular::receiveSMS(std::string messageNumber) {

	std::string command("AT+QCMGR=" + messageNumber + "\r");

	auto ret = cmux->GetChannel("Commands")->SendCommandResponse(
			command.c_str(), 2, 2000);

	bool messageParsed = false;

	std::string messageRawBody;
	UTF8 receivedNumber;
	if (ret.size() != 0) {
		for (uint32_t i = 0; i < ret.size(); i++) {
			if (ret[i].find("QCMGR") != std::string::npos) {


				std::istringstream ss(ret[i]);
				std::string token;
				std::vector<std::string> tokens;
				while (std::getline(ss, token, ',')) {
					tokens.push_back(token);
				}
				tokens[1].erase(
						std::remove(tokens[1].begin(), tokens[1].end(), '\"'),
						tokens[1].end());
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
				//parse sender number
				receivedNumber = UCS2(tokens[1]).toUTF8();

				//parse date
				tokens[3].erase(std::remove(tokens[3].begin(), tokens[3].end(), '\"'), tokens[3].end());
				struct tm tm;
				strptime((tokens[3] + " " + tokens[4]).c_str(), "%y/%m/%d %H:%M:%S", &tm);
				auto messageDate = mktime(&tm);
				//if its single message process
				if (tokens.size() == 5) {
					//todo add message to database

					messageRawBody = ret[i+1];
					messageParsed = true;
				}
				//if its concatenated message wait for last message
				else if (tokens.size() == 8) {

					uint32_t last = std::stoi(tokens[7]);
					uint32_t current = std::stoi(tokens[6]);

					if (current == last) {
						messageParts.push_back(ret[i + 1]);

						for (uint32_t j = 0; j < messageParts.size(); j++) {
							messageRawBody += messageParts[j];
						}
						messageParts.clear();
						messageParsed = true;
					} else {
						messageParts.push_back(ret[i + 1]);
					}
				}
				if (messageParsed)
				{
					messageParsed = false;

					UTF8 decodedMessage = UCS2(messageRawBody).toUTF8();

					SMSRecord record;
					record.body = decodedMessage;
					record.number = receivedNumber;
					record.type = SMSType::INBOX;
					record.isRead = true;
					record.date = messageDate;

					DBServiceAPI::SMSAdd(this, record);
				}
			}
		}
	}
	//delete message from modem memory
	cmux->GetChannel("Commands")->SendCommandResponse(
			("AT+CMGD=" + messageNumber).c_str(), 1, 150);
	return true;
}
