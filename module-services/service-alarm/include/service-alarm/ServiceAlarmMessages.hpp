// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmObject.hpp"
#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <Service/SystemReturnCodes.hpp>
#include <time/time_conversion.hpp>

#include <memory>
#include <chrono>

class AlarmMessage : public sys::DataMessage
{
  public:
    AlarmMessage(std::shared_ptr<AlarmObject> alarm) : sys::DataMessage(MessageType::AlarmMessage), alarm(alarm)
    {}

    virtual ~AlarmMessage() = default;

    std::shared_ptr<AlarmObject> getAlarm() const
    {
        return alarm;
    }

  private:
    std::shared_ptr<AlarmObject> alarm;
};

class AlarmSetMessage : public AlarmMessage
{
  public:
    explicit AlarmSetMessage(std::shared_ptr<AlarmObject> alarm) : AlarmMessage(alarm)
    {}
};

class AlarmTriggeredMessage : public AlarmMessage
{
  public:
    explicit AlarmTriggeredMessage(std::shared_ptr<AlarmObject> alarm) : AlarmMessage(alarm)
    {}
};

class AlarmResponseMessage : public sys::ResponseMessage
{
  public:
    AlarmResponseMessage(sys::ReturnCodes retCode, std::shared_ptr<AlarmObject> alarm)
        : sys::ResponseMessage(retCode, MessageType::AlarmMessage), alarm(alarm)
    {}

    virtual ~AlarmResponseMessage() = default;

    std::shared_ptr<AlarmObject> getAlarm() const
    {
        return alarm;
    }

  private:
    std::shared_ptr<AlarmObject> alarm;
};
