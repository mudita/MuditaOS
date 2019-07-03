/*
 *  @file CommunicationMuxChannel.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 28.06.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_COMMUNICATIONMUXCHANNEL_HPP
#define PUREPHONE_COMMUNICATIONMUXCHANNEL_HPP

#include "MuxChannel.hpp"

class CommunicationMuxChannel : public MuxChannel {
public:

    CommunicationMuxChannel(MuxDaemon* mux);
    ~CommunicationMuxChannel();



    int ParseInMessage(MuxChannelMsg* msg) override final;

    ssize_t SendCommand(const char* cmd,uint32_t timeout = 1000);
};


#endif //PUREPHONE_COMMUNICATIONMUXCHANNEL_HPP
