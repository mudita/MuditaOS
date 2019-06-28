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


NotificationMuxChannel::NotificationMuxChannel(MuxDaemon* mux):
    MuxChannel(mux,1,MuxChannel::MuxChannelType ::Notification,"NotificationChannel")
{

}

NotificationMuxChannel::~NotificationMuxChannel() {

}

int NotificationMuxChannel::ParseInMessage(MuxChannel::MuxChannelMsg *msg) {

    // Incoming call
    if (msg->m_data.find("RING") != std::string::npos) {
        //TODO:M.P incoming call URC action
        LOG_DEBUG((name + ": incoming call...").c_str());
        char* resp = "ATA\r\n";
        mux->WriteMuxFrame(GetChannelNumber(), reinterpret_cast<unsigned char *>(resp),strlen(resp), static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH));
    }

    // Incoming call
    if (msg->m_data.find("+CRING") != std::string::npos) {
        //TODO:M.P incoming call URC action
        LOG_DEBUG((name + ": incoming call...").c_str());
    }

    // Received new SMS
    if (msg->m_data.find("+CMT: ") != std::string::npos) {
        //TODO:M.P handle new SMS
        LOG_DEBUG((name + ": received new SMS").c_str());
    }
}
