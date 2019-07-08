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

int NotificationMuxChannel::ParseInputData(uint8_t* data, size_t size) {

    std::string msgStr(data,data+size);

    // Incoming call
    if (msgStr.find("RING") != std::string::npos) {
        //TODO:M.P incoming call URC action
        LOG_TRACE((name + ": incoming call...").c_str());
        char* resp = "ATA\r";
        mux->WriteMuxFrame(GetChannelNumber(), reinterpret_cast<unsigned char *>(resp),strlen(resp), static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH));
    }

    // Incoming call
    if (msgStr.find("+CRING") != std::string::npos) {
        //TODO:M.P incoming call URC action
        LOG_TRACE((name + ": incoming call...").c_str());
    }

    // Call aborted/failed
    if (msgStr.find("NO CARRIER") != std::string::npos) {
        //TODO:M.P handle it
        LOG_TRACE((name + ": call failed/aborted").c_str());
    }

    // Received new SMS
    if (msgStr.find("+CMTI: ") != std::string::npos) {
        //TODO:M.P handle new SMS
        LOG_TRACE((name + ": received new SMS notification").c_str());
    }

    // Received signal strength change
    if (msgStr.find("+QIND: \"csq\"") != std::string::npos) {
        LOG_TRACE((name + ": received signal strength change notification").c_str());
    }

    return 0;
}
