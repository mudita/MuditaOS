
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

MuxChannel::MuxChannel(MuxDaemon *mux,uint32_t logicalNumber,const char *name) :
        v24signals(
                static_cast<int >(MuxDefines::GSM0710_SIGNAL_DV ) | static_cast<int >(MuxDefines::GSM0710_SIGNAL_RTR) |
                static_cast<int >(MuxDefines::GSM0710_SIGNAL_RTC) | static_cast<int >(MuxDefines::GSM0710_EA)),
        frameAllowed(1),
        disc_ua_pending(0),
        logicalNumber(logicalNumber),
        name(name),
        state(State::Closed),
        mux(mux){

    // Send virtrual channel request frame to GSM modem
    mux->WriteMuxFrame(logicalNumber,NULL,0, static_cast<unsigned char>(MuxDefines ::GSM0710_TYPE_SABM) | static_cast<unsigned char>(MuxDefines ::GSM0710_PF));

}

MuxChannel::~MuxChannel() {

}