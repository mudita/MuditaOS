/*
 *  @file NotificationMuxChannel.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 28.06.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_NOTIFICATIONMUXCHANNEL_HPP
#define PUREPHONE_NOTIFICATIONMUXCHANNEL_HPP

#include "MuxChannel.hpp"

class MuxDaemon;

class NotificationMuxChannel : public MuxChannel{
public:

    NotificationMuxChannel(MuxDaemon* mux);
    ~NotificationMuxChannel();



    int ParseInputData(uint8_t* data, size_t size) override final;
};


#endif //PUREPHONE_NOTIFICATIONMUXCHANNEL_HPP
