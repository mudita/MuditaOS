// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <time/time_locale.hpp>

class TimeMessage : public sys::DataMessage
{
  public:
    explicit TimeMessage(MessageType messageType) : sys::DataMessage(messageType){};

    virtual ~TimeMessage() = default;
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

    class SetAutomaticDateAndTimeRequest : public sys::DataMessage
    {
      public:
        explicit SetAutomaticDateAndTimeRequest(const bool value)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), value(value){};
        auto getValue() const -> bool
        {
            return value;
        }

      private:
        bool value;
    };

    class AutomaticDateAndTimeChangedMessage : public sys::DataMessage
    {
      public:
        explicit AutomaticDateAndTimeChangedMessage(const bool value)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), value(value){};
        auto getValue() const -> bool
        {
            return value;
        }

      private:
        bool value;
    };

    class SetTimeFormatRequest : public sys::DataMessage
    {
      public:
        explicit SetTimeFormatRequest(const utils::time::Locale::TimeFormat timeFormat)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), timeFormat(timeFormat){};
        auto getTimeFormat() const -> utils::time::Locale::TimeFormat
        {
            return timeFormat;
        };

      private:
        utils::time::Locale::TimeFormat timeFormat;
    };

    class SetDateFormatRequest : public sys::DataMessage
    {
      public:
        explicit SetDateFormatRequest(const utils::time::Locale::DateFormat dateFormat)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), dateFormat(dateFormat){};
        auto getDateFormat() const -> utils::time::Locale::DateFormat
        {
            return dateFormat;
        };

      private:
        utils::time::Locale::DateFormat dateFormat;
    };

    class SetTimezoneRequest : public sys::DataMessage
    {
      public:
        explicit SetTimezoneRequest(const std::string &timezoneName)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), timezone(timezoneName)
        {}
        auto getTimezoneName() const -> std::string
        {
            return timezone;
        }

      private:
        std::string timezone;
    };

    class GetAutomaticDateAndTimeRequest : public sys::DataMessage
    {
      public:
        explicit GetAutomaticDateAndTimeRequest() : sys::DataMessage(MessageType::MessageTypeUninitialized)
        {}
    };

    class GetAutomaticDateAndTimeResponse : public sys::ResponseMessage
    {
      public:
        explicit GetAutomaticDateAndTimeResponse(bool settingValue)
            : sys::ResponseMessage(), isAutomaticDateAndTimeOn(settingValue)
        {}
        auto isAutomaticDateAndTime() const noexcept
        {
            return isAutomaticDateAndTimeOn;
        }

      private:
        bool isAutomaticDateAndTimeOn = false;
    };
} // namespace stm::message
