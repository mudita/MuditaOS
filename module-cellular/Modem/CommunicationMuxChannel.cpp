/*
 *  @file CommunicationMuxChannel.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 28.06.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "CommunicationMuxChannel.hpp"
#include "MuxDaemon.hpp"


CommunicationMuxChannel::CommunicationMuxChannel(MuxDaemon* mux):
        MuxChannel(mux,2,MuxChannel::MuxChannelType ::Communication,"CommunicationChannel")
{

}

CommunicationMuxChannel::~CommunicationMuxChannel() {

}

int CommunicationMuxChannel::ParseInMessage(MuxChannel::MuxChannelMsg *msg) {
    LOG_FATAL((name + " received message: " + msg->m_data).c_str());
}

ssize_t CommunicationMuxChannel::SendCommand(const char *cmd, uint32_t timeout) {
    //mux->WritesiMuxFrame(GetChannelNumber(), cmd,strlen(cmd), static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH));
}