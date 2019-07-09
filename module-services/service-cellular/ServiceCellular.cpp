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

    testTimerID = CreateTimer(3000, true);
    ReloadTimer(testTimerID);

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
    char *resp = "AT\r";
    //muxdaemon->WriteMuxFrame(2, reinterpret_cast<unsigned char *>(resp),strlen(resp), static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH));
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
        default:
            break;
    }
}