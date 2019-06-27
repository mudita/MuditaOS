
/*
 * @file MuxChannel.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "MuxChannel.hpp"
#include "GSM0710.hpp"
#include "MuxDaemon.hpp"

MuxChannel::MuxChannel(MuxDaemon *mux, uint32_t logicalNumber, const char *name,uint32_t stackDepth) :
        cpp_freertos::Thread(std::string{name}, stackDepth/4 /* Stack depth in bytes */, 0),
        workerMsgQueue(this),
        v24signals(
                static_cast<int >(MuxDefines::GSM0710_SIGNAL_DV ) | static_cast<int >(MuxDefines::GSM0710_SIGNAL_RTR) |
                static_cast<int >(MuxDefines::GSM0710_SIGNAL_RTC) | static_cast<int >(MuxDefines::GSM0710_EA)),
        frameAllowed(1),
        disc_ua_pending(0),
        logicalNumber(logicalNumber),
        name(name),
        state(State::Closed),
        mux(mux) {


}

MuxChannel::~MuxChannel() {

}

int MuxChannel::Open() {

    enableRunLoop = true;
    Start();

    // Send virtual channel request frame to GSM modem
    mux->WriteMuxFrame(logicalNumber, NULL, 0, static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_SABM) |
                                               static_cast<unsigned char>(MuxDefines::GSM0710_PF));
}

int MuxChannel::Close() {

    if (mux->inputBuffer->cmux_mode)
        mux->WriteMuxFrame(logicalNumber, NULL, 0, static_cast<unsigned char>(MuxDefines::GSM0710_CONTROL_CLD) |
                                                   static_cast<unsigned char>(MuxDefines::GSM0710_CR));
    else
        mux->WriteMuxFrame(logicalNumber, mux->closeChannelCmd, sizeof(mux->closeChannelCmd),
                           static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH));
}


void MuxChannel::Run() {

    while(enableRunLoop){
        auto msg = workerMsgQueue.pop();

        LOG_DEBUG("Received msg size(%d) : %s",msg->m_size,msg->m_data);


    }
}

int MuxChannel::Send(uint8_t* data, size_t size) {

    workerMsgQueue.push(std::make_shared<MuxChannelMsg>(data,size));
    return size;
}