/*
 *  @file ServiceCellular.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 03.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>

#include "ServiceCellular.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

#include "MessageType.hpp"

#include "messages/CellularMessage.hpp"
#include <ticks.hpp>

#include "Common.hpp"
#include "log/log.hpp"

const char *ServiceCellular::serviceName = "ServiceCellular";
constexpr int32_t ServiceCellular::signalStrengthToDB[];


ServiceCellular::ServiceCellular()
        : sys::Service(serviceName, "", 8192UL, sys::ServicePriority::Idle) {

    LOG_INFO("[ServiceCellular] Initializing");

    busChannels.push_back(sys::BusChannels::ServiceCellularNotifications);

    callStateTimer = CreateTimer(1000, true);

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
            case CellularNotificationMessage::Type::None:
                // no data field is used
                LOG_ERROR("unexpected notification");
                break;

            case CellularNotificationMessage::Type::IncomingCall:
                msg->data = message;
                break;

            case CellularNotificationMessage::Type::NewIncomingSMS:
                //TODO:M.P fill message's fields
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

// Invoked when timer ticked
void ServiceCellular::TickHandler(uint32_t id) {
    std::shared_ptr<CellularRequestMessage> msg = std::make_shared<CellularRequestMessage>(
            MessageType::CellularListCurrentCalls);

    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, this);
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

sys::Message_t ServiceCellular::DataReceivedHandler(sys::DataMessage *msgl,sys::ResponseMessage* resp) {
    std::shared_ptr<sys::ResponseMessage> responseMsg;

    switch (static_cast<MessageType >(msgl->messageType)) {

        // Incoming notifications from Notification Virtual Channel
        case MessageType::CellularNotification: {
            CellularNotificationMessage *msg = reinterpret_cast<CellularNotificationMessage *>(msgl);

            if ((msg->type == CellularNotificationMessage::Type::CallAborted) ||
                (msg->type == CellularNotificationMessage::Type::CallBusy)) {
                stopTimer(callStateTimer);
            } else if (msg->type == CellularNotificationMessage::Type::PowerUpProcedureComplete) {
                sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartConfProcedure),
                                      GetName(), this);
                state = State ::ModemConfigurationInProgress;
            } else {
                //ignore rest of notifications
            }
        }
            break;

        case MessageType::CellularStartPowerUpProcedure: {
                   auto powerRet = cmux->PowerUpProcedure();
                   if (powerRet == TS0710::ConfState::Success) {
                       sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartConfProcedure),
                                             GetName(), this);
                   }
                   else if(powerRet == TS0710::ConfState::PowerUp){
                       state = State::PowerUpInProgress;

                   }
                   else{
                       LOG_FATAL("[ServiceCellular] PowerUp procedure failed");
                       state = State::Failed;
                   }
               }
                   break;

               case MessageType::CellularStartConfProcedure: {
                   // Start configuration procedure, if it's first run modem will be restarted
                   auto confRet = cmux->ConfProcedure();
                   if (confRet == TS0710::ConfState::Success) {
                       sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartAudioConfProcedure),
                                             GetName(), this);
                       state = State::AudioConfigurationInProgress;
                   }
                   else {
                       LOG_FATAL("[ServiceCellular] Initialization failed, not ready");
                       state = State::Failed;
                   }
               }
                   break;

               case MessageType ::CellularStartAudioConfProcedure:
               {
                   auto audioRet = cmux->AudioConfProcedure();
                   if(audioRet == TS0710::ConfState::Success){

                       char buf[32];
                       LOG_DEBUG("Setting baudrate %i baud", ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
                       sprintf(buf, "AT+IPR=%i\r", ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
                       if (!cmux->CheckATCommandResponse(cmux->getParser()->SendCommand(buf, 1))) {
                           LOG_ERROR("Baudrate setup error");
                           state = State::Failed;
                           break;
                       }
                       cmux->getCellular()->SetSpeed(ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);

                       vTaskDelay(1000);

                       if (cmux->StartMultiplexer() == TS0710::ConfState::Success)
                       {
                           LOG_DEBUG("[ServiceCellular] Modem is fully operational");

                           DLC_channel *notificationsChannel = cmux->GetChannel("Notifications"); //open channel - notifications
                           if (notificationsChannel)
                           {
                               LOG_DEBUG("Setting up notifications callback");
                               notificationsChannel->setCallback(notificationCallback);
                           }

                           state = State::Ready;
                        }
                        else {
                            LOG_DEBUG("[ServiceCellular] Modem FAILED");
                            state = State::Failed;
                        }

                       state = State::Ready;
                       // Propagate "ServiceReady" notification into system
                       sys::Bus::SendMulticast(std::make_shared<CellularNotificationMessage>(
                               CellularNotificationMessage::Type::ServiceReady),
                               sys::BusChannels::ServiceCellularNotifications, this);
                   }
                   else if(audioRet == TS0710::ConfState::Failure){
                       sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularStartAudioConfProcedure),
                                             GetName(), this);
                   }
                   else{
                       // Reset procedure started, do nothing here
                       state = State::PowerUpInProgress;
                   }
               }
               break;

        case MessageType::CellularListCurrentCalls: {
            auto ret = cmux->GetChannel("Commands")->SendCommandResponse("AT+CLCC\r", 3, 300);
            if(cmux->CheckATCommandResponse(ret)) 
            {
                auto beg = ret[1].find(",", 0);
                beg = ret[1].find(",", beg + 1);
                // If call changed to "Active" state stop callStateTimer(used for polling for call state)
                if (std::stoul(ret[1].substr(beg + 1, 1)) == static_cast<uint32_t >(CallStates::Active)) {
                    auto msg = std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallActive);
                    sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceCellularNotifications, this);

                    stopTimer(callStateTimer);
                }

                responseMsg = std::make_shared<CellularResponseMessage>(true);
            } else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
            break;

        case MessageType::CellularHangupCall: {
            auto ret = cmux->GetChannel("Commands")->SendCommandResponse("ATH\r", 1, 5000); 
            if(cmux->CheckATCommandResponse(ret)) {
                responseMsg = std::make_shared<CellularResponseMessage>(true);
            } else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
            stopTimer(callStateTimer);

            // Propagate "CallAborted" notification into system
                sys::Bus::SendMulticast(std::make_shared<CellularNotificationMessage>(
                CellularNotificationMessage::Type::CallAborted),
                sys::BusChannels::ServiceCellularNotifications, this);
        }
            break;

        case MessageType::CellularAnswerIncomingCall: {
            // TODO: per Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf timeout should be possibly set up to 90s
            auto ret = cmux->GetChannel("Commands")->SendCommandResponse("ATA\r", 1, 5000); 
            if(cmux->CheckATCommandResponse(ret))
            {
                responseMsg = std::make_shared<CellularResponseMessage>(true);
                // Propagate "CallActive" notification into system
                sys::Bus::SendMulticast(std::make_shared<CellularNotificationMessage>(
                        CellularNotificationMessage::Type::CallActive),
                        sys::BusChannels::ServiceCellularNotifications, this);
            } else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
            break;

        case MessageType::CellularDialNumber: {
            CellularRequestMessage *msg = reinterpret_cast<CellularRequestMessage *>(msgl);
            auto ret = cmux->GetChannel("Commands")->SendCommandResponse(("ATD" + msg->data + ";\r").c_str(), 1, 5000);
            if(cmux->CheckATCommandResponse(ret))
            {
                responseMsg = std::make_shared<CellularResponseMessage>(true);
                // activate call state timer
                ReloadTimer(callStateTimer);
                // Propagate "Ringing" notification into system
                sys::Bus::SendMulticast(std::make_shared<CellularNotificationMessage>(
                    CellularNotificationMessage::Type::Ringing, msg->data),
                    sys::BusChannels::ServiceCellularNotifications, this);
            } else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }

            
        }
            break;

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

    return CellularNotificationMessage::Type::None;

}
