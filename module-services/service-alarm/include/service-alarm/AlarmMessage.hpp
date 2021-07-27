// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-calendar/data/dateCommon.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/EventRecord.hpp>
#include <MessageType.hpp>
#include <Service/Message.hpp>

#include <vector>

namespace alarms
{

    class AlarmMessage : public sys::DataMessage
    {
      public:
        AlarmMessage() : sys::DataMessage(MessageType::AlarmMessage){};
    };

    class AlarmGetRequestMessage : public AlarmMessage
    {
      public:
        AlarmGetRequestMessage(const unsigned int id = 1) : id(id){};
        unsigned int id;
    };

    class AlarmGetResponseMessage : public AlarmMessage
    {
      public:
        AlarmGetResponseMessage(const std::vector<AlarmEventRecord> alarms = std::vector<AlarmEventRecord>())
            : alarms(alarms){};
        std::vector<AlarmEventRecord> alarms;
    };

    class AlarmAddRequestMessage : public AlarmMessage
    {
      public:
        AlarmAddRequestMessage(const AlarmEventRecord &alarmEvent = AlarmEventRecord()) : alarmEvent(alarmEvent){};
        AlarmEventRecord alarmEvent;
    };

    class AlarmAddResponseMessage : public AlarmMessage
    {
      public:
        AlarmAddResponseMessage(const AlarmEventRecord &alarmEvent = AlarmEventRecord()) : alarmEvent(alarmEvent){};
        AlarmEventRecord alarmEvent;
    };

    class AlarmUpdateRequestMessage : public AlarmMessage
    {
      public:
        AlarmUpdateRequestMessage(const AlarmEventRecord &alarmEvent = AlarmEventRecord()) : alarmEvent(alarmEvent){};
        AlarmEventRecord alarmEvent;
    };

    class AlarmUpdateResponseMessage : public AlarmMessage
    {
      public:
        AlarmUpdateResponseMessage(const AlarmEventRecord &alarmEvent = AlarmEventRecord()) : alarmEvent(alarmEvent){};
        AlarmEventRecord alarmEvent;
    };

    class AlarmRemoveRequestMessage : public AlarmMessage
    {
      public:
        AlarmRemoveRequestMessage(const unsigned int id = 0) : id(id){};
        unsigned int id;
    };

    class AlarmRemoveResponseMessage : public AlarmMessage
    {
      public:
        AlarmRemoveResponseMessage(const bool success = false) : success(success){};
        bool success;
    };

    class AlarmsGetInRangeRequestMessage : public AlarmMessage
    {
      public:
        AlarmsGetInRangeRequestMessage(
            const std::chrono::time_point<std::chrono::system_clock> start = TIME_POINT_INVALID,
            const std::chrono::time_point<std::chrono::system_clock> end   = TIME_POINT_INVALID)
            : start(start), end(end){};
        std::chrono::time_point<std::chrono::system_clock> start;
        std::chrono::time_point<std::chrono::system_clock> end;
    };

    class AlarmsGetInRangeResponseMessage : public AlarmMessage
    {
      public:
        AlarmsGetInRangeResponseMessage(std::vector<AlarmEventRecord> alarms = std::vector<AlarmEventRecord>())
            : alarms(alarms){};
        std::vector<AlarmEventRecord> alarms;
    };

    class AlarmGetNextSingleEventsRequestMessage : public AlarmMessage
    {
      public:
        AlarmGetNextSingleEventsRequestMessage(){};
    };

    class AlarmGetNextSingleEventsResponseMessage : public AlarmMessage
    {
      public:
        AlarmGetNextSingleEventsResponseMessage(
            std::vector<SingleEventRecord> singleEvents = std::vector<SingleEventRecord>())
            : singleEvents(singleEvents){};
        std::vector<SingleEventRecord> singleEvents;
    };

} // namespace alarms
