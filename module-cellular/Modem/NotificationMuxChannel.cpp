/*
 *  @file NotificationMuxChannel.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 28.06.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "NotificationMuxChannel.hpp"
#include <string.h>
#include "MuxDaemon.hpp"

class Service;

NotificationMuxChannel::NotificationMuxChannel(InOutSerialWorker* inout,NotificationCallback_t callback):
    MuxChannel(inout,MuxChannel::MuxChannelType ::Notification,"NotificationChannel"),
    notificationCallback(std::move(callback))
{

}

NotificationMuxChannel::~NotificationMuxChannel() {

}

int NotificationMuxChannel::ParseInputData(uint8_t* data, size_t size) {

    std::string msgStr(data,data+size);

#if 0 // M.P: those notifications are not used, instead we're using +CLIP URC as incoming call notification
    // Additionaly we can fetch caller's number from it
    // Incoming call
    if (msgStr.find("RING") != std::string::npos) {
        LOG_TRACE((name + ": incoming call...").c_str());
        notificationCallback(NotificationType::IncomingCall,"dummy");// TODO:M.P add incoming number parsing
    }

    // Incoming call
    if (msgStr.find("+CRING") != std::string::npos) {
        LOG_TRACE((name + ": incoming call...").c_str());
        notificationCallback(NotificationType::IncomingCall,"dummy");// TODO:M.P add incoming number parsing
    }
#endif

    // Incoming call
    if (auto ret = msgStr.find("+CLIP: ") != std::string::npos) {
        LOG_TRACE((name + ": incoming call...").c_str());

        auto beg = msgStr.find("\"",ret);
        auto end = msgStr.find("\"",ret + beg+1);
        notificationCallback(NotificationType::IncomingCall,msgStr.substr(beg+1,end-beg-1));
    }


    // Call aborted/failed
    if (msgStr.find("NO CARRIER") != std::string::npos) {
        LOG_TRACE((name + ": call failed/aborted").c_str());
        notificationCallback(NotificationType::CallAborted,"");//
    }

    // Call busy
    if (msgStr.find("BUSY") != std::string::npos) {
        LOG_TRACE((name + ": call busy").c_str());
        notificationCallback(NotificationType::CallBusy,"");//
    }

    // Received new SMS
    if (msgStr.find("+CMTI: ") != std::string::npos) {
        LOG_TRACE((name + ": received new SMS notification").c_str());
        notificationCallback(NotificationType::NewIncomingSMS,"888777333");// TODO:M.P add SMS nr parsing
    }

    // Received signal strength change
    if (auto ret = msgStr.find("+QIND: \"csq\"") != std::string::npos) {
        LOG_TRACE((name + ": received signal strength change notification").c_str());
        auto beg = msgStr.find(",",ret);
        auto end = msgStr.find(",",ret + beg+1);
        notificationCallback(NotificationType::SignalStrengthUpdate,msgStr.substr(beg+1,end-beg-1));
    }

    return 0;
}
