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
#include <functional>
#include <variant>

class MuxDaemon;

enum class NotificationType{
    IncomingCall,
    CallAborted,
    NewIncomingSMS,
    SignalStrengthUpdate
};


class NotificationMuxChannel : public MuxChannel{
public:

    using NotificationCallback_t =  std::function<void(NotificationType type, std::string resp)>;

    NotificationMuxChannel(MuxDaemon* mux,NotificationCallback_t callback);
    ~NotificationMuxChannel();



    int ParseInputData(uint8_t* data, size_t size) override final;

private:
    NotificationCallback_t notificationCallback;
};


#endif //PUREPHONE_NOTIFICATIONMUXCHANNEL_HPP
