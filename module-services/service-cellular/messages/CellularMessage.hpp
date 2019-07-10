/*
 *  @file CellularMessage.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 03.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_CELLULARMESSAGE_HPP
#define PUREPHONE_CELLULARMESSAGE_HPP

#include <memory>
#include <variant>
#include "Service/Message.hpp"
#include "MessageType.hpp"


class CellularMessage : public sys::DataMessage {
public:
    CellularMessage(MessageType messageType) : sys::DataMessage(static_cast<uint32_t>(messageType)),
                                               type(messageType) {};

    virtual ~CellularMessage() {};

    MessageType type;

};

class CellularNotificationMessage : public CellularMessage {
public:

    enum class Type {
        IncomingCall,
        CallAborted,
        NewIncomingSMS,
        SignalStrengthUpdate

    };


    CellularNotificationMessage(Type type) : CellularMessage(
            MessageType::CellularNotification), type(type){}

    ~CellularNotificationMessage() {}

    Type type;
    std::string data;
    uint32_t signalStrength;
    int32_t dBmSignalStrength;

};

class CellularRequestMessage : public CellularMessage{
public:

    CellularRequestMessage(MessageType messageType):CellularMessage(messageType){}
    ~CellularRequestMessage() {}

    std::string data;

};

class CellularResponseMessage: public sys::ResponseMessage {
public:
    CellularResponseMessage(uint32_t retCode) : sys::ResponseMessage(),retCode(retCode) {};
    virtual ~CellularResponseMessage() {};

    uint32_t retCode;
};


#endif //PUREPHONE_CELLULARMESSAGE_HPP
