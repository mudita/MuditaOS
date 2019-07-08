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

NotificationMuxChannel::NotificationMuxChannel(MuxDaemon* mux,NotificationCallback_t callback):
    MuxChannel(mux,1,MuxChannel::MuxChannelType ::Notification,"NotificationChannel"),
    notificationCallback(std::move(callback))
{

}

NotificationMuxChannel::~NotificationMuxChannel() {

}

int NotificationMuxChannel::ParseInputData(uint8_t* data, size_t size) {

    std::string msgStr(data,data+size);

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

    // Call aborted/failed
    if (msgStr.find("NO CARRIER") != std::string::npos) {
        LOG_TRACE((name + ": call failed/aborted").c_str());
        notificationCallback(NotificationType::CallAborted,"dummy");//
    }

    // Received new SMS
    if (msgStr.find("+CMTI: ") != std::string::npos) {
        LOG_TRACE((name + ": received new SMS notification").c_str());
        notificationCallback(NotificationType::NewIncomingSMS,"dummy");// TODO:M.P add SMS nr parsing
    }

    // Received signal strength change
    if (msgStr.find("+QIND: \"csq\"") != std::string::npos) {
        LOG_TRACE((name + ": received signal strength change notification").c_str());
        notificationCallback(NotificationType::SignalStrengthUpdate,"dummy");// TODO:M.P add signal strength parsing
    }

    return 0;
}
