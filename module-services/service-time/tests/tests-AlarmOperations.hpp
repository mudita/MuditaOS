// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <service-time/AlarmHandler.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <AlarmOperations.hpp>
#include <AlarmRepository.hpp>

class MockAlarmHandler : public alarms::AlarmHandler
{
  public:
    MOCK_METHOD(bool, handle, (const AlarmEventRecord &record), ());
    MOCK_METHOD(bool, handleOff, (const AlarmEventRecord &record), ());
};

class MockAlarmEventsRepository : public alarms::AbstractAlarmEventsRepository
{
  public:
    std::vector<AlarmEventRecord> nextRecords;

    MOCK_METHOD(void,
                getAlarmEvent,
                (const std::uint32_t alarmId, const alarms::OnGetAlarmEventCallback &callback),
                ());
    MOCK_METHOD(void,
                getAlarmEvents,
                (std::uint32_t offset, std::uint32_t limit, const alarms::OnGetAlarmEventsCallback &callback),
                ());
    MOCK_METHOD(void, toggleAll, (const bool toggle, const alarms::OnToggleAll &callback), ());

    auto addAlarmEvent(const AlarmEventRecord &alarmEvent, const alarms::OnAddAlarmEventCallback &callback) -> void
    {
        addSingleEvent(alarmEvent);
        callback({true});
    }

    auto updateAlarmEvent(const AlarmEventRecord &alarmEvent, const alarms::OnAddAlarmEventCallback &callback) -> void
    {
        nextRecords.erase(std::remove_if(nextRecords.begin(),
                                         nextRecords.end(),
                                         [&alarmEvent](const AlarmEventRecord &ae) { return ae.ID == alarmEvent.ID; }),
                          nextRecords.end());
        addSingleEvent(alarmEvent);
        callback({true});
    }
    auto removeAlarmEvent(const std::uint32_t alarmId, const alarms::OnRemoveAlarmEventCallback &callback) -> void
    {
        nextRecords.erase(std::remove_if(nextRecords.begin(),
                                         nextRecords.end(),
                                         [&alarmId](const AlarmEventRecord &ae) { return ae.ID == alarmId; }),
                          nextRecords.end());
        callback({true});
    }

    void getAlarmEventsInRange(std::uint32_t offset,
                               std::uint32_t limit,
                               const alarms::OnGetAlarmEventsInRangeCallback &callback)
    {
        callback({nextRecords, nextRecords.size()});
    }

    void getAlarmEnabledEvents(const alarms::OnGetAlarmEventsCallback &callback)
    {
        std::vector<AlarmEventRecord> result;
        for (const auto &rec : nextRecords) {
            if (rec.enabled) {
                result.push_back(rec);
            }
        }
        callback({result});
    }

    void addSingleEvent(const AlarmEventRecord record)
    {
        nextRecords.push_back(record);
    }
};

namespace
{
    /// Time point from string with current test machine time shift
    TimePoint TimePointFromStringWithShift(std::string timeString)
    {
        const auto timeToCalc = TimePointFromString("2000-01-01 12:00:00");
        const auto fromTimeT  = std::chrono::system_clock::to_time_t(timeToCalc);
        const auto fromLocal  = std::localtime(&fromTimeT);
        fromLocal->tm_hour    = 12;
        fromLocal->tm_min     = 0;
        auto time             = TimePointFloorMinutes(std::chrono::system_clock::from_time_t(std::mktime(fromLocal)));
        auto currentTimeShift = timeToCalc - time;

        return TimePointFromString(timeString.c_str()) - currentTimeShift;
    }

} // namespace

class AlarmOperationsFixture : public ::testing::Test
{
  protected:
    virtual std::unique_ptr<alarms::IAlarmOperations> getMockedAlarmOperations(
        std::unique_ptr<MockAlarmEventsRepository> &alarmRepo);
};

constexpr auto defRRule   = "";
constexpr auto defMusic   = "";
constexpr auto defEnabled = true;
constexpr auto defSnooze  = 15;
