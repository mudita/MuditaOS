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
#include "Modem/NotificationMuxChannel.hpp"


class CellularMessage : public sys::DataMessage {
public:
    CellularMessage() = delete;
    CellularMessage(MessageType messageType) : sys::DataMessage(static_cast<uint32_t>(messageType)),
                                               type(messageType) {};

    virtual ~CellularMessage() = default;

    MessageType type;

};

class CellularNotificationMessage : public CellularMessage {
public:

    CellularNotificationMessage() = delete;
    CellularNotificationMessage(NotificationType type, const std::string & data = "") : CellularMessage(
            MessageType::CellularNotification), type(type), data(data) {}

    virtual ~CellularNotificationMessage() = default;

    NotificationType type;
    std::string data;
    uint32_t signalStrength=0;
    int32_t dBmSignalStrength=0;

};

class CellularRequestMessage : public CellularMessage{
public:

    CellularRequestMessage() = delete;
    CellularRequestMessage(MessageType messageType):CellularMessage(messageType){}
    virtual ~CellularRequestMessage() = default;

    std::string data;

};

class CellularResponseMessage: public sys::ResponseMessage {
public:
    CellularResponseMessage(bool retCode = true) : sys::ResponseMessage(),retCode(retCode) {};
    virtual ~CellularResponseMessage() = default;

    bool retCode;
};


#endif //PUREPHONE_CELLULARMESSAGE_HPP
