/*
 *  @file InternetMessage.hpp
 *  @author Łukasz Skrzypczak (lukasz.skrzypczak@mudita.com)
 *  @date 16.09.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_INTERNETMESSAGE_HPP
#define PUREPHONE_INTERNETMESSAGE_HPP

#include <memory>
#include <variant>
#include "Service/Message.hpp"
#include "MessageType.hpp"

class InternetMessage : public sys::DataMessage
{
  public:
    InternetMessage(MessageType messageType) : sys::DataMessage(messageType), type(messageType){};

    virtual ~InternetMessage(){};

    MessageType type;
};

class InternetNotificationMessage : public InternetMessage
{
  public:
    enum class Type
    {
        Configured,
        Connected,
        Disconnected,
        RequestProcessed,
        ServiceReady, // Idle state of the service. This is a start state before any call is initialized by user or by
                      // network. service returns to this state when call is finished.
    };

    InternetNotificationMessage(Type type) : InternetMessage(MessageType::InternetNotification), type(type)
    {}

    ~InternetNotificationMessage()
    {}

    Type type;
    std::string data;
};

class InternetRequestMessage : public InternetMessage
{
  public:
    enum class Type
    {
        Connect,    // data service connected
        Disconnect, // data service disconnected
        Configure,
    };

    InternetRequestMessage(MessageType messageType) : InternetMessage(messageType)
    {}
    ~InternetRequestMessage()
    {}

    std::string data;
    std::string apn;
    std::string user;
    std::string password;
    std::string url;
    std::string request;
};

class InternetResponseMessage : public sys::ResponseMessage
{
  public:
    InternetResponseMessage(uint32_t retCode) : sys::ResponseMessage(), retCode(retCode){};
    virtual ~InternetResponseMessage(){};

    uint32_t retCode;
    std::vector<std::string> retData;
};

#endif // PUREPHONE_INTERNETMESSAGE_HPP
