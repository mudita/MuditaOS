// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

class TimeMessage : public sys::DataMessage
{
  public:
    explicit TimeMessage(MessageType messageType) : sys::DataMessage(messageType){};

    virtual ~TimeMessage() = default;
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
    virtual ~TimeResponseMessage() = default;

    bool retCode;
    std::string data;
};

namespace stm::message
{
    class UpdateRTCValueFromTmMessage : public sys::DataMessage
    {
      public:
        explicit UpdateRTCValueFromTmMessage(const struct tm &time)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), time(time){};

        auto getTime() -> struct tm &
        {
            return time;
        }

      private:
        struct tm time;
    };

    class UpdateRTCValueFromTimestampMessage : public sys::DataMessage
    {
      public:
        explicit UpdateRTCValueFromTimestampMessage(const time_t &time)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), time(time){};

        auto getTime() -> time_t &
        {
            return time;
        }

      private:
        time_t time;
    };

    class UpdateTimeZoneMessage : public sys::DataMessage
    {
      public:
        explicit UpdateTimeZoneMessage(const std::string &timezone)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), timezone(timezone){};

        auto getTimezone() -> std::string
        {
            return timezone;
        }

      private:
        std::string timezone;
    };

    class TimeChangeRequestMessage : public sys::DataMessage
    {
      public:
        explicit TimeChangeRequestMessage(const time_t &time)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), time(time){};

        auto getTime() -> time_t
        {
            return time;
        }

      private:
        time_t time;
    };
} // namespace stm::message
