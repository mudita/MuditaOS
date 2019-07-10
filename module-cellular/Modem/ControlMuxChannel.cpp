/*
 *  @file ControlMuxChannel.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 28.06.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "ControlMuxChannel.hpp"


ControlMuxChannel::ControlMuxChannel(MuxDaemon *mux) :
        MuxChannel(mux, 0, MuxChannel::MuxChannelType::Control, "ControlChannel") {

}

ControlMuxChannel::~ControlMuxChannel() {

}

