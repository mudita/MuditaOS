// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/EventRecord.hpp>
#include <MessageType.hpp>
#include <Service/Message.hpp>

#include <time/dateCommon.hpp>

#include <vector>

namespace alarms
{

    class AlarmMessage : public sys::DataMessage
    {
      public:
        AlarmMessage() : sys::DataMessage(MessageType::AlarmMessage){};
    };

    class AlarmResponse : public sys::ResponseMessage
    {};

    class AlarmGetRequestMessage : public AlarmMessage
    {
      public:
        AlarmGetRequestMessage(const unsigned int id = 1) : id(id){};
        const unsigned int id;
    };

    class AlarmGetResponseMessage : public AlarmResponse
    {
      public:
        AlarmGetResponseMessage(const AlarmEventRecord alarm = AlarmEventRecord{}) : alarm(alarm){};
        const AlarmEventRecord alarm;
    };

    class AlarmAddRequestMessage : public AlarmMessage
    {
      public:
        AlarmAddRequestMessage(const AlarmEventRecord &alarmEvent = AlarmEventRecord()) : alarmEvent(alarmEvent){};
        const AlarmEventRecord alarmEvent;
    };

    class AlarmAddResponseMessage : public AlarmResponse
    {
      public:
        AlarmAddResponseMessage(const bool success = false) : success(success){};
        const bool success;
    };

    class AlarmUpdateRequestMessage : public AlarmMessage
    {
      public:
        AlarmUpdateRequestMessage(const AlarmEventRecord &alarmEvent = AlarmEventRecord()) : alarmEvent(alarmEvent){};
        AlarmEventRecord alarmEvent;
    };

    class AlarmUpdateResponseMessage : public AlarmResponse
    {
      public:
        AlarmUpdateResponseMessage(const bool success = false) : success(success){};
        const bool success;
    };

    class AlarmRemoveRequestMessage : public AlarmMessage
    {
      public:
        AlarmRemoveRequestMessage(const unsigned int id = 0) : id(id){};
        const unsigned int id;
    };

    class AlarmRemoveResponseMessage : public AlarmResponse
    {
      public:
        AlarmRemoveResponseMessage(const bool success = false) : success(success){};
        const bool success;
    };

    class AlarmToggleAllRequestMessage : public AlarmMessage
    {
      public:
        AlarmToggleAllRequestMessage(const bool toggle) : toggle(toggle){};
        const bool toggle;
    };

    class AlarmToggleAllResponseMessage : public AlarmResponse
    {
      public:
        AlarmToggleAllResponseMessage(const bool success = false) : success(success){};
        const bool success;
    };

    class AlarmsGetInRangeRequestMessage : public AlarmMessage
    {
      public:
        AlarmsGetInRangeRequestMessage(
            const std::chrono::time_point<std::chrono::system_clock> start = TIME_POINT_INVALID,
            const std::chrono::time_point<std::chrono::system_clock> end   = TIME_POINT_INVALID,
            std::uint32_t offset                                           = 0,
            std::uint32_t limit                                            = 0)
            : start(start), end(end), offset(offset), limit(limit){};
        const std::chrono::time_point<std::chrono::system_clock> start;
        const std::chrono::time_point<std::chrono::system_clock> end;
        const std::uint32_t offset;
        const std::uint32_t limit;
    };

    class AlarmsGetInRangeResponseMessage : public AlarmResponse
    {
      public:
        explicit AlarmsGetInRangeResponseMessage(std::pair<std::vector<AlarmEventRecord>, std::uint32_t> p)
            : alarms(std::move(p.first)), count(p.second){};
        std::vector<AlarmEventRecord> alarms;
        const std::uint32_t count = 0;
    };

    class AlarmGetFirstNextSingleEventRequestMessage : public AlarmMessage
    {};

    class AlarmGetFirstNextSingleEventResponseMessage : public AlarmResponse
    {
      public:
        AlarmGetFirstNextSingleEventResponseMessage(SingleEventRecord singleEvent = {}) : singleEvent(singleEvent){};
        SingleEventRecord singleEvent;
    };

    class AlarmGetNextSingleEventsRequestMessage : public AlarmMessage
    {
      public:
        AlarmGetNextSingleEventsRequestMessage(){};
    };

    class AlarmGetNextSingleEventsResponseMessage : public AlarmResponse
    {
      public:
        AlarmGetNextSingleEventsResponseMessage(
            std::vector<SingleEventRecord> singleEvents = std::vector<SingleEventRecord>())
            : singleEvents(singleEvents){};
        std::vector<SingleEventRecord> singleEvents;
    };

    class RingingAlarmTurnOffRequestMessage : public AlarmMessage
    {
      public:
        explicit RingingAlarmTurnOffRequestMessage(const std::uint32_t id = 0) : id(id){};
        const std::uint32_t id;
    };

    class RingingAlarmTurnOffResponseMessage : public AlarmResponse
    {
      public:
        explicit RingingAlarmTurnOffResponseMessage(const bool success = false) : success(success){};
        const bool success;
    };

    class TurnOffSnoozeRequestMessage : public AlarmMessage
    {
      public:
        explicit TurnOffSnoozeRequestMessage(const std::uint32_t id = 0) : id(id){};
        const std::uint32_t id;
    };

    class TurnOffSnoozeResponseMessage : public AlarmResponse
    {
      public:
        explicit TurnOffSnoozeResponseMessage(const bool success = false) : success(success){};
        const bool success;
    };

    class RingingAlarmSnoozeRequestMessage : public AlarmMessage
    {
      public:
        RingingAlarmSnoozeRequestMessage(const std::uint32_t id, const TimePoint nextAlarmTime = TIME_POINT_INVALID)
            : id(id), nextAlarmTime(nextAlarmTime){};
        const std::uint32_t id;
        const TimePoint nextAlarmTime;
    };

    class RingingAlarmSnoozeResponseMessage : public AlarmResponse
    {
      public:
        explicit RingingAlarmSnoozeResponseMessage(const bool success = false) : success(success){};
        const bool success;
    };

    class PostponeSnoozeRequestMessage : public AlarmMessage
    {
      public:
        PostponeSnoozeRequestMessage(const std::uint32_t id, const TimePoint nextAlarmTime = TIME_POINT_INVALID)
            : id(id), nextAlarmTime(nextAlarmTime){};
        const std::uint32_t id;
        const TimePoint nextAlarmTime;
    };

    class PostponeSnoozeResponseMessage : public AlarmResponse
    {
      public:
        explicit PostponeSnoozeResponseMessage(const bool success = false) : success(success){};
        const bool success;
    };

    class StopAllSnoozedAlarmsRequestMessage : public AlarmMessage
    {
      public:
        explicit StopAllSnoozedAlarmsRequestMessage(){};
    };

    class RegisterSnoozedAlarmsCountChangeHandlerRequestMessage : public AlarmMessage
    {
      public:
        explicit RegisterSnoozedAlarmsCountChangeHandlerRequestMessage(){};
    };

    class SnoozedAlarmsCountChangeMessage : public AlarmMessage
    {
      public:
        explicit SnoozedAlarmsCountChangeMessage(unsigned snoozedCount) : snoozedCount{snoozedCount} {};

        [[nodiscard]] unsigned getSnoozedCount() const
        {
            return snoozedCount;
        }

      private:
        unsigned snoozedCount = 0;
    };

    /// Message to register service (sender) awaiting for ActiveAlarmMessage
    class RegisterActiveAlarmsIndicatorHandlerRequestMessage : public AlarmMessage
    {
      public:
        explicit RegisterActiveAlarmsIndicatorHandlerRequestMessage(){};
    };

    /// Message to indicate if any alarm is currently actively waiting for execution
    class ActiveAlarmMessage : public AlarmMessage
    {
      public:
        explicit ActiveAlarmMessage(bool anyAlarmActive) : anyAlarmActive{anyAlarmActive} {};

        [[nodiscard]] bool isAnyAlarmActive() const noexcept
        {
            return anyAlarmActive;
        }

      private:
        bool anyAlarmActive = 0;
    };

    class GetSnoozedAlarmsRequestMessage : public AlarmMessage
    {
      public:
        GetSnoozedAlarmsRequestMessage(){};
    };

    class GetSnoozedAlarmsResponseMessage : public AlarmResponse
    {
      public:
        explicit GetSnoozedAlarmsResponseMessage(std::vector<SingleEventRecord> snoozedAlarms)
            : snoozedAlarms(std::move(snoozedAlarms)){};
        const std::vector<SingleEventRecord> snoozedAlarms;
    };
} // namespace alarms
