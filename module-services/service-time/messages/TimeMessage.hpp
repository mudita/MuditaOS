/*
 *  @file TimeMessage.hpp
 *  @author
 *  @date 08.05.20
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_TIMEMESSAGE_HPP
#define PUREPHONE_TIMEMESSAGE_HPP

#include <memory>
#include <variant>
#include "Service/Message.hpp"
#include "MessageType.hpp"

class TimeMessage : public sys::DataMessage
{
  public:
    TimeMessage(MessageType messageType) : sys::DataMessage(messageType), type(messageType){};

    virtual ~TimeMessage() = default;

    MessageType type = MessageType::MessageTypeUninitialized;
};

class TimersProcessingStartMessage : public TimeMessage
{
  public:
    TimersProcessingStartMessage() : TimeMessage(MessageType::TimersProcessingStart)
    {}
};

class TimersProcessingStopMessage : public TimeMessage
{
  public:
    TimersProcessingStopMessage() : TimeMessage(MessageType::TimersProcessingStop)
    {}
};

class ReloadTimersMessage : public TimeMessage
{
  public:
    ReloadTimersMessage() : TimeMessage(MessageType::ReloadTimers)
    {}
};

class TimeResponseMessage : public sys::ResponseMessage
{
  public:
    TimeResponseMessage(bool retCode,
                        std::string retdata    = std::string(),
                        MessageType responseTo = MessageType::MessageTypeUninitialized)
        : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode), data(retdata){};
    TimeResponseMessage(bool retCode, MessageType responseTo)
        : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode){};
    virtual ~TimeResponseMessage(){};

    bool retCode;
    std::string data;
};

#endif // PUREPHONE_SERVICETIME_HPP
