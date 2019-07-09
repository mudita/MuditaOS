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


    CellularNotificationMessage(Type type, const std::string &data) : CellularMessage(
            MessageType::CellularNotification), type(type), data(data) {}

    ~CellularNotificationMessage() {}

    Type type;
    std::string data;

};


#endif //PUREPHONE_CELLULARMESSAGE_HPP
