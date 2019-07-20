/*
 *  @file ControlMuxChannel.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 28.06.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_CONTROLMUXCHANNEL_HPP
#define PUREPHONE_CONTROLMUXCHANNEL_HPP

#include "MuxChannel.hpp"

class ControlMuxChannel : public MuxChannel  {
public:

    ControlMuxChannel(InOutSerialWorker* inout);
    ~ControlMuxChannel();

};


#endif //PUREPHONE_CONTROLMUXCHANNEL_HPP
