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

const char *ServiceCellular::serviceName = "ServiceCellular";
constexpr int32_t ServiceCellular::signalStrengthToDB[];


ServiceCellular::ServiceCellular()
        : sys::Service(serviceName, 1024 * 4, sys::ServicePriority::Idle) {
    LOG_INFO("[ServiceCellular] Initializing");

    //testTimerID = CreateTimer(3000, true);
    //ReloadTimer(testTimerID);

    muxdaemon = MuxDaemon::Create([this](NotificationType type, std::string resp) {

        auto msg = std::make_shared<CellularNotificationMessage>(static_cast<CellularNotificationMessage::Type >(type));

        switch (type) {

            case NotificationType::IncomingCall:
                msg->data = resp;
                break;

            case NotificationType::CallAborted:
                //TODO:M.P dummy ?

                break;

            case NotificationType::NewIncomingSMS:
                //TODO:M.P fill message's fields
                break;

            case NotificationType::SignalStrengthUpdate:
                msg->signalStrength = std::stoll(resp);
                msg->dBmSignalStrength =
                        msg->signalStrength > (sizeof(signalStrengthToDB) / sizeof(signalStrengthToDB[0]))
                        ? signalStrengthToDB[0] : signalStrengthToDB[msg->signalStrength];
                break;

        }


        sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceCellularNotifications, this);

    });


/*    vTaskDelay(5000);
    muxdaemon.reset();

    muxdaemon = MuxDaemon::Create();*/

}

ServiceCellular::~ServiceCellular() {

    LOG_INFO("[ServiceCellular] Cleaning resources");
}

// Invoked when timer ticked
void ServiceCellular::TickHandler(uint32_t id) {

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

        case MessageType ::CellularHangupCall:
        {
            CellularRequestMessage *msg = reinterpret_cast<CellularRequestMessage *>(msgl);
            auto ret = muxdaemon->SendCommandReponse(MuxChannel::MuxChannelType::Communication,"ATH\r",1);
            if((ret.size() == 1) && (ret[0] == "OK")){
                responseMsg = std::make_shared<CellularResponseMessage>(true);
            }else{
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }


        }
            break;

        case MessageType ::CellularAnswerIncomingCall:
        {
            CellularRequestMessage *msg = reinterpret_cast<CellularRequestMessage *>(msgl);
            auto ret = muxdaemon->SendCommandReponse(MuxChannel::MuxChannelType::Communication,"ATA\r",1);
            if((ret.size() == 1) && (ret[0] == "OK")){
                responseMsg = std::make_shared<CellularResponseMessage>(true);
            }else{
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
            break;

        case MessageType ::CellularDialNumber:
        {
            CellularRequestMessage *msg = reinterpret_cast<CellularRequestMessage *>(msgl);
            auto ret = muxdaemon->SendCommandReponse(MuxChannel::MuxChannelType::Communication,("ATD" + msg->data + ";\r").c_str(),1);
            if((ret.size() == 1) && (ret[0] == "OK")){
                responseMsg = std::make_shared<CellularResponseMessage>(true);
            }else{
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
            break;




        default:
            break;
    }


    return responseMsg;
}