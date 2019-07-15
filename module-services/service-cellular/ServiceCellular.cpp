/*
 *  @file ServiceCellular.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 03.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "ServiceCellular.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

#include "Modem/MuxDaemon.hpp"

#include "MessageType.hpp"

#include "messages/CellularMessage.hpp"

#include "Common.hpp"

const char *ServiceCellular::serviceName = "ServiceCellular";
constexpr int32_t ServiceCellular::signalStrengthToDB[];


ServiceCellular::ServiceCellular()
        : sys::Service(serviceName, 1024, sys::ServicePriority::Idle) {
    LOG_INFO("[ServiceCellular] Initializing");

    busChannels.push_back(sys::BusChannels::ServiceCellularNotifications);

    callStateTimer = CreateTimer(1000, true);

    notificationCallback = [this](NotificationType type, std::string resp) {

        auto msg = std::make_shared<CellularNotificationMessage>(static_cast<CellularNotificationMessage::Type >(type));

        switch (type) {

            case NotificationType ::CallBusy:
            case NotificationType ::CallActive:
            case NotificationType::CallAborted:
                // no data field is used
                break;

            case NotificationType::IncomingCall:
                msg->data = resp;
                break;

            case NotificationType::NewIncomingSMS:
                //TODO:M.P fill message's fields
                break;

            case NotificationType::SignalStrengthUpdate:
                msg->signalStrength = std::stoll(resp);
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


    muxdaemon = MuxDaemon::Create(notificationCallback);


/*    vTaskDelay(5000);
    muxdaemon.reset();

    muxdaemon = MuxDaemon::Create();*/

}

ServiceCellular::~ServiceCellular() {

    LOG_INFO("[ServiceCellular] Cleaning resources");
}

// Invoked when timer ticked
void ServiceCellular::TickHandler(uint32_t id) {
    std::shared_ptr<CellularRequestMessage> msg = std::make_shared<CellularRequestMessage>(
            MessageType::CellularListCurrentCalls);

    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, this);
}

// Invoked during initialization
sys::ReturnCodes ServiceCellular::InitHandler() {

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceCellular::DeinitHandler() {

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceCellular::WakeUpHandler() {
    return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceCellular::SleepHandler() {
    return sys::ReturnCodes::Success;
}

sys::Message_t ServiceCellular::DataReceivedHandler(sys::DataMessage *msgl) {
    std::shared_ptr<sys::ResponseMessage> responseMsg;

    switch (static_cast<MessageType >(msgl->messageType)) {

        // Incoming notifications from Notification Virtual Channel
        case MessageType::CellularNotification: {
            CellularNotificationMessage *msg = reinterpret_cast<CellularNotificationMessage *>(msgl);

            if ((msg->type == CellularNotificationMessage::Type::CallAborted) ||
                (msg->type == CellularNotificationMessage::Type::CallBusy)) {
                stopTimer(callStateTimer);
            } else {
                //ignore the rest of notifications
            }
        }
            break;


        case MessageType::CellularListCurrentCalls: {
            auto ret = muxdaemon->SendCommandResponse(MuxChannel::MuxChannelType::Communication, "AT+CLCC\r", 3);
            if ((ret.size() == 3) && (ret[2] == "OK")) {

                auto beg = ret[1].find(",",0);
                beg = ret[1].find(",",beg+1);
                // If call changed to "Active" state stop callStateTimer(used for polling for call state)
                if(std::stoul(ret[1].substr(beg+1,1)) == static_cast<uint32_t >(CallStates::Active)){
                    notificationCallback(NotificationType::CallActive,"");
                    stopTimer(callStateTimer);
                }

                responseMsg = std::make_shared<CellularResponseMessage>(true);
            } else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
            break;

        case MessageType::CellularHangupCall: {
            CellularRequestMessage *msg = reinterpret_cast<CellularRequestMessage *>(msgl);
            auto ret = muxdaemon->SendCommandResponse(MuxChannel::MuxChannelType::Communication, "ATH\r", 1);
            if ((ret.size() == 1) && (ret[0] == "OK")) {
                responseMsg = std::make_shared<CellularResponseMessage>(true);
            } else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
            stopTimer(callStateTimer);

        }
            break;

        case MessageType::CellularAnswerIncomingCall: {
            CellularRequestMessage *msg = reinterpret_cast<CellularRequestMessage *>(msgl);
            auto ret = muxdaemon->SendCommandResponse(MuxChannel::MuxChannelType::Communication, "ATA\r", 1);
            if ((ret.size() == 1) && (ret[0] == "OK")) {
                responseMsg = std::make_shared<CellularResponseMessage>(true);
            } else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
            break;

        case MessageType::CellularDialNumber: {
            CellularRequestMessage *msg = reinterpret_cast<CellularRequestMessage *>(msgl);
            auto ret = muxdaemon->SendCommandResponse(MuxChannel::MuxChannelType::Communication,
                                                      ("ATD" + msg->data + ";\r").c_str(), 1);
            if ((ret.size() == 1) && (ret[0] == "OK")) {
                responseMsg = std::make_shared<CellularResponseMessage>(true);
                // activate call state timer
                ReloadTimer(callStateTimer);
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