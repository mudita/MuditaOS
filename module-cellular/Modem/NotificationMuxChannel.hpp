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
class InOutSerialWorker;

enum class NotificationType{
    IncomingCall, 			    //device receives connection from other device.
    CallAborted,			    //user tried to call other device but receiving side dropped call or call unsuccessful
    CallBusy,				    //user tried to call other device but receiving side dropped call or call unsuccessful
                                //receiving CallAborted or CallBusy depends on actual network/BTS which GSM modem is connected to
    CallActive,				    //call is in progress both if call was initialized by user and when user received incoming call.
    Ringing,				    //user provided number to call to and service initialized calling procedure.
    NewIncomingSMS,			    //device received new sms from network. (what about sms delivery reports?).
    SignalStrengthUpdate,	    //update of the strength of the network's signal.
    PowerUpProcedureComplete,   //power up procedure complete
    ServiceReady,			    //Idle state of the service. This is a start state before any call is initialized by user or by network.
                                //service returns to this state when call is finished.
};


class NotificationMuxChannel : public MuxChannel{
public:

    using NotificationCallback_t =  std::function<void(NotificationType type, std::string resp)>;

    NotificationMuxChannel(InOutSerialWorker* inout,NotificationCallback_t callback);
    ~NotificationMuxChannel();



    int ParseInputData(uint8_t* data, size_t size) override final;

private:
    NotificationCallback_t notificationCallback;
};


#endif //PUREPHONE_NOTIFICATIONMUXCHANNEL_HPP
