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
        AlarmMessage(MessageType type) : sys::DataMessage(type){};
    };

    class AlarmGetRequestMessage : public AlarmMessage
    {
      public:
        AlarmGetRequestMessage(const unsigned int id) : AlarmMessage(MessageType::AlarmGetRequest), id(id){};
        unsigned int id;
    };

    class AlarmGetResponseMessage : public AlarmMessage
    {
      public:
        AlarmGetResponseMessage(const std::vector<AlarmEventRecord> alarms)
            : AlarmMessage(MessageType::AlarmGetResponse), alarms(alarms){};
        std::vector<AlarmEventRecord> alarms;
    };

    class AlarmAddRequestMessage : public AlarmMessage
    {
      public:
        AlarmAddRequestMessage(const AlarmEventRecord &alarmEvent)
            : AlarmMessage(MessageType::AlarmAddRequest), alarmEvent(alarmEvent){};
        AlarmEventRecord alarmEvent;
    };

    class AlarmAddResponseMessage : public AlarmMessage
    {
      public:
        AlarmAddResponseMessage(const AlarmEventRecord &alarmEvent)
            : AlarmMessage(MessageType::AlarmAddResponse), alarmEvent(alarmEvent){};
        AlarmEventRecord alarmEvent;
    };

    class AlarmUpdateRequestMessage : public AlarmMessage
    {
      public:
        AlarmUpdateRequestMessage(const AlarmEventRecord &alarmEvent)
            : AlarmMessage(MessageType::AlarmUpdateRequest), alarmEvent(alarmEvent){};
        AlarmEventRecord alarmEvent;
    };

    class AlarmUpdateResponseMessage : public AlarmMessage
    {
      public:
        AlarmUpdateResponseMessage(const AlarmEventRecord &alarmEvent)
            : AlarmMessage(MessageType::AlarmUpdateResponse), alarmEvent(alarmEvent){};
        AlarmEventRecord alarmEvent;
    };

    class AlarmRemoveRequestMessage : public AlarmMessage
    {
      public:
        AlarmRemoveRequestMessage(const unsigned int id) : AlarmMessage(MessageType::AlarmRemoveRequest), id(id){};
        unsigned int id;
    };

    class AlarmRemoveResponseMessage : public AlarmMessage
    {
      public:
        AlarmRemoveResponseMessage(const bool success)
            : AlarmMessage(MessageType::AlarmRemoveResponse), success(success){};
        bool success;
    };

    class AlarmsGetInRangeRequestMessage : public AlarmMessage
    {
      public:
        AlarmsGetInRangeRequestMessage(const TimePoint start, const TimePoint end)
            : AlarmMessage(MessageType::AlarmsGetInRangeRequest), start(start), end(end){};
        TimePoint start;
        TimePoint end;
    };

    class AlarmsGetInRangeResponseMessage : public AlarmMessage
    {
      public:
        AlarmsGetInRangeResponseMessage(std::vector<AlarmEventRecord> alarms)
            : AlarmMessage(MessageType::AlarmsGetInRangeResponse), alarms(alarms){};
        std::vector<AlarmEventRecord> alarms;
    };

    class AlarmGetNextSingleEventsRequestMessage : public AlarmMessage
    {
      public:
        AlarmGetNextSingleEventsRequestMessage() : AlarmMessage(MessageType::AlarmGetNextSingleEventsRequest){};
    };

    class AlarmGetNextSingleEventsResponseMessage : public AlarmMessage
    {
      public:
        AlarmGetNextSingleEventsResponseMessage(std::vector<SingleEventRecord> singleEvents)
            : AlarmMessage(MessageType::AlarmGetNextSingleEventsResponse), singleEvents(singleEvents){};
        std::vector<SingleEventRecord> singleEvents;
    };

} // namespace alarms
