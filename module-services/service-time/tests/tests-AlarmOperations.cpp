// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <AlarmOperations.hpp>
#include <AlarmRepository.hpp>
#include <service-time/AlarmHandler.hpp>

#include <module-db/Interface/AlarmEventRecord.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
    std::vector<AlarmEventRecord> recurringRecords;

    MOCK_METHOD(void,
                getAlarmEvent,
                (const std::uint32_t alarmId, const alarms::OnGetAlarmEventCallback &callback),
                ());
    MOCK_METHOD(void,
                getAlarmEvents,
                (std::uint32_t offset, std::uint32_t limit, const alarms::OnGetAlarmEventsCallback &callback),
                ());
    MOCK_METHOD(void,
                getAlarmEventsInRange,
                (TimePoint start,
                 TimePoint end,
                 std::uint32_t offset,
                 std::uint32_t limit,
                 const alarms::OnGetAlarmEventsInRangeCallback &callback),
                ());

    auto addAlarmEvent(const AlarmEventRecord &alarmEvent, const alarms::OnAddAlarmEventCallback &callback) -> void
    {
        addSingleEvent(alarmEvent);
        if (!alarmEvent.rruleText.empty()) {
            addRecurringEvent(alarmEvent);
        }
        callback({true});
    }

    auto updateAlarmEvent(const AlarmEventRecord &alarmEvent, const alarms::OnAddAlarmEventCallback &callback) -> void
    {
        nextRecords.erase(std::remove_if(nextRecords.begin(),
                                         nextRecords.end(),
                                         [&alarmEvent](const AlarmEventRecord &ae) { return ae.ID == alarmEvent.ID; }),
                          nextRecords.end());
        recurringRecords.erase(
            std::remove_if(recurringRecords.begin(),
                           recurringRecords.end(),
                           [&alarmEvent](const AlarmEventRecord &ae) { return ae.ID == alarmEvent.ID; }),
            recurringRecords.end());

        addSingleEvent(alarmEvent);
        if (!alarmEvent.rruleText.empty()) {
            addRecurringEvent(alarmEvent);
        }
        callback({true});
    }
    auto removeAlarmEvent(const std::uint32_t alarmId, const alarms::OnRemoveAlarmEventCallback &callback) -> void
    {
        nextRecords.erase(std::remove_if(nextRecords.begin(),
                                         nextRecords.end(),
                                         [&alarmId](const AlarmEventRecord &ae) { return ae.ID == alarmId; }),
                          nextRecords.end());
        recurringRecords.erase(std::remove_if(recurringRecords.begin(),
                                              recurringRecords.end(),
                                              [&alarmId](const AlarmEventRecord &ae) { return ae.ID == alarmId; }),
                               recurringRecords.end());
        callback({true});
    }

    void getAlarmEventsRecurringInRange(TimePoint start,
                                        TimePoint end,
                                        std::uint32_t offset,
                                        std::uint32_t limit,
                                        const alarms::OnGetAlarmEventsRecurringInRange &callback)
    {
        callback({recurringRecords});
    }

    void getNext(TimePoint start, std::uint32_t offset, std::uint32_t limit, const alarms::OnGetNextCallback &callback)
    {
        callback({nextRecords});
    }

    void addSingleEvent(const AlarmEventRecord record)
    {
        nextRecords.push_back(record);
    }

    void addRecurringEvent(const AlarmEventRecord record)
    {
        addSingleEvent(record);
        recurringRecords.push_back(record);
    }
};

alarms::IAlarmOperations::GetCurrentTime timeInjector = []() { return TimePointFromString("2022-11-11 05:00:00"); };
alarms::IAlarmOperations::OnUpdateAlarmProcessed universalBoolCallback = [](bool success) { EXPECT_EQ(success, true); };

class AlarmOperationsFixture : public ::testing::Test
{
  protected:
    auto getMockedAlarmOperations(std::unique_ptr<MockAlarmEventsRepository> &alarmRepo)
    {
        return std::make_unique<alarms::AlarmOperationsCommon>(std::move(alarmRepo), timeInjector);
    }
};

constexpr auto defName     = "";
constexpr auto defDuration = 60;
constexpr auto defAllDay   = false;
constexpr auto defRRule    = "";
constexpr auto defMusic    = "";
constexpr auto defEnabled  = true;
constexpr auto defSnooze   = 15;

TEST_F(AlarmOperationsFixture, getNextOneEvent)
{
    alarms::IAlarmOperations::OnGetNextSingleProcessed callback = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 1);
        EXPECT_EQ(records[0].startDate, TimePointFromString("2022-11-11 09:00:00"));
        EXPECT_EQ(records[0].parent->ID, 1);
    };

    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(1,
                                                          defName,
                                                          TimePointFromString("2022-11-11 09:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->getNextSingleEvents(TimePointFromString("2020-01-01 00:00:00"), callback);
}

TEST_F(AlarmOperationsFixture, getNextTwoEvents)
{
    alarms::IAlarmOperations::OnGetNextSingleProcessed callback = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 1);
        EXPECT_EQ(records[0].startDate, TimePointFromString("2022-11-11 09:00:00"));
        EXPECT_EQ(records[0].parent->ID, 1);
    };

    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->addSingleEvent(AlarmEventRecord(1,
                                                   defName,
                                                   TimePointFromString("2022-11-11 09:00:00"),
                                                   defDuration,
                                                   defAllDay,
                                                   defRRule,
                                                   defMusic,
                                                   defEnabled,
                                                   defSnooze));

    alarmRepoMock->addSingleEvent(AlarmEventRecord(2,
                                                   defName,
                                                   TimePointFromString("2022-12-11 02:00:00"),
                                                   defDuration,
                                                   defAllDay,
                                                   defRRule,
                                                   defMusic,
                                                   defEnabled,
                                                   defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->getNextSingleEvents(TimePointFromString("2020-01-01 00:00:00"), callback);
}

TEST_F(AlarmOperationsFixture, getNextTwoEventsReversedOrder)
{
    alarms::IAlarmOperations::OnGetNextSingleProcessed callback = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 1);
        EXPECT_EQ(records[0].startDate, TimePointFromString("2022-11-11 09:00:00"));
        EXPECT_EQ(records[0].parent->ID, 2);
    };

    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->addSingleEvent(AlarmEventRecord(1,
                                                   defName,
                                                   TimePointFromString("2022-12-11 02:00:00"),
                                                   defDuration,
                                                   defAllDay,
                                                   defRRule,
                                                   defMusic,
                                                   defEnabled,
                                                   defSnooze));

    alarmRepoMock->addSingleEvent(AlarmEventRecord(2,
                                                   defName,
                                                   TimePointFromString("2022-11-11 09:00:00"),
                                                   defDuration,
                                                   defAllDay,
                                                   defRRule,
                                                   defMusic,
                                                   defEnabled,
                                                   defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->getNextSingleEvents(TimePointFromString("2020-01-01 00:00:00"), callback);
}

TEST_F(AlarmOperationsFixture, getNextWithRecursiveFirst)
{
    alarms::IAlarmOperations::OnGetNextSingleProcessed callback = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 1);
        EXPECT_EQ(records[0].startDate, TimePointFromString("2022-01-11 02:00:00"));
        EXPECT_EQ(records[0].parent->ID, 1);
    };

    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();

    alarmRepoMock->addRecurringEvent(AlarmEventRecord(1,
                                                      defName,
                                                      TimePointFromString("2022-01-11 02:00:00"),
                                                      defDuration,
                                                      defAllDay,
                                                      "FREQ=DAILY",
                                                      defMusic,
                                                      defEnabled,
                                                      defSnooze));
    alarmRepoMock->addSingleEvent(AlarmEventRecord(2,
                                                   defName,
                                                   TimePointFromString("2022-11-11 09:00:00"),
                                                   defDuration,
                                                   defAllDay,
                                                   defRRule,
                                                   defMusic,
                                                   defEnabled,
                                                   defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->getNextSingleEvents(TimePointFromString("2020-01-01 00:00:00"), callback);
}

TEST_F(AlarmOperationsFixture, getNextWithRecursiveBeforeRange)
{
    alarms::IAlarmOperations::OnGetNextSingleProcessed callback = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 1);
        EXPECT_EQ(records[0].startDate, TimePointFromString("2022-10-01 02:00:00"));
        EXPECT_EQ(records[0].parent->ID, 1);
    };

    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();

    alarmRepoMock->addRecurringEvent(AlarmEventRecord(1,
                                                      defName,
                                                      TimePointFromString("2022-01-11 02:00:00"),
                                                      defDuration,
                                                      defAllDay,
                                                      "FREQ=DAILY",
                                                      defMusic,
                                                      defEnabled,
                                                      defSnooze));

    alarmRepoMock->addSingleEvent(AlarmEventRecord(2,
                                                   defName,
                                                   TimePointFromString("2022-11-11 09:00:00"),
                                                   defDuration,
                                                   defAllDay,
                                                   defRRule,
                                                   defMusic,
                                                   defEnabled,
                                                   defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->getNextSingleEvents(TimePointFromString("2022-10-01 00:00:00"), callback);
}

TEST_F(AlarmOperationsFixture, getNextWithTwoRecursive)
{
    alarms::IAlarmOperations::OnGetNextSingleProcessed callback1 = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 1);
        EXPECT_EQ(records[0].startDate, TimePointFromString("2022-10-01 09:00:00"));
        EXPECT_EQ(records[0].parent->ID, 2);
    };
    alarms::IAlarmOperations::OnGetNextSingleProcessed callback2 = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 1);
        EXPECT_EQ(records[0].startDate, TimePointFromString("2022-10-02 02:00:00"));
        EXPECT_EQ(records[0].parent->ID, 1);
    };

    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->addRecurringEvent(AlarmEventRecord(1,
                                                      defName,
                                                      TimePointFromString("2022-01-11 02:00:00"),
                                                      defDuration,
                                                      defAllDay,
                                                      "FREQ=DAILY",
                                                      defMusic,
                                                      defEnabled,
                                                      defSnooze));
    alarmRepoMock->addRecurringEvent(AlarmEventRecord(2,
                                                      defName,
                                                      TimePointFromString("2022-01-11 09:00:00"),
                                                      defDuration,
                                                      defAllDay,
                                                      "FREQ=DAILY",
                                                      defMusic,
                                                      defEnabled,
                                                      defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->getNextSingleEvents(TimePointFromString("2022-10-01 03:00:00"), callback1);
    alarmOperations->getNextSingleEvents(TimePointFromString("2022-10-01 17:00:00"), callback2);
}

TEST_F(AlarmOperationsFixture, getNextMultipleEvents)
{
    alarms::IAlarmOperations::OnGetNextSingleProcessed callback = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 2);
        EXPECT_EQ(records[0].startDate, TimePointFromString("2022-01-01 02:00:00"));
        EXPECT_EQ(records[0].parent->ID, 1);
        EXPECT_EQ(records[1].startDate, TimePointFromString("2022-01-01 02:00:00"));
        EXPECT_EQ(records[1].parent->ID, 3);
    };

    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->addSingleEvent(AlarmEventRecord(1,
                                                   defName,
                                                   TimePointFromString("2022-01-01 02:00:00"),
                                                   defDuration,
                                                   defAllDay,
                                                   defRRule,
                                                   defMusic,
                                                   defEnabled,
                                                   defSnooze));

    alarmRepoMock->addSingleEvent(AlarmEventRecord(2,
                                                   defName,
                                                   TimePointFromString("2022-11-11 09:00:00"),
                                                   defDuration,
                                                   defAllDay,
                                                   defRRule,
                                                   defMusic,
                                                   defEnabled,
                                                   defSnooze));
    alarmRepoMock->addSingleEvent(AlarmEventRecord(3,
                                                   defName,
                                                   TimePointFromString("2022-01-01 02:00:00"),
                                                   defDuration,
                                                   defAllDay,
                                                   defRRule,
                                                   defMusic,
                                                   defEnabled,
                                                   defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->getNextSingleEvents(TimePointFromString("2020-01-01 00:00:00"), callback);
}

TEST_F(AlarmOperationsFixture, getNextMultipleEventsWithRecursive)
{
    alarms::IAlarmOperations::OnGetNextSingleProcessed callback = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 2);
        EXPECT_EQ(records[0].startDate, TimePointFromString("2022-01-01 02:00:00"));
        EXPECT_EQ(records[0].parent->ID, 1);
        EXPECT_EQ(records[1].startDate, TimePointFromString("2022-01-01 02:00:00"));
        EXPECT_EQ(records[1].parent->ID, 3);
    };

    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->addSingleEvent(AlarmEventRecord(1,
                                                   defName,
                                                   TimePointFromString("2022-01-01 02:00:00"),
                                                   defDuration,
                                                   defAllDay,
                                                   defRRule,
                                                   defMusic,
                                                   defEnabled,
                                                   defSnooze));

    alarmRepoMock->addRecurringEvent(AlarmEventRecord(2,
                                                      defName,
                                                      TimePointFromString("2021-01-01 09:00:00"),
                                                      defDuration,
                                                      defAllDay,
                                                      "FREQ=MONTHLY;BYMONTHDAY=1;INTERVAL=1",
                                                      defMusic,
                                                      defEnabled,
                                                      defSnooze));
    alarmRepoMock->addRecurringEvent(AlarmEventRecord(3,
                                                      defName,
                                                      TimePointFromString("2021-01-01 02:00:00"),
                                                      defDuration,
                                                      defAllDay,
                                                      "FREQ=MONTHLY;BYMONTHDAY=1;INTERVAL=1",
                                                      defMusic,
                                                      defEnabled,
                                                      defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->getNextSingleEvents(TimePointFromString("2022-01-01 00:00:00"), callback);
}

TEST_F(AlarmOperationsFixture, handleFirstEvent)
{
    alarms::IAlarmOperations::OnGetNextSingleProcessed callback = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 1);
        EXPECT_EQ(records[0].startDate, TimePointFromString("2022-11-11 09:00:00"));
        EXPECT_EQ(records[0].parent->ID, 1);
    };

    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(1,
                                                          defName,
                                                          TimePointFromString("2022-11-11 11:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(2,
                                                          defName,
                                                          TimePointFromString("2022-11-11 09:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 09:00:00"));
}

TEST_F(AlarmOperationsFixture, handleEventAfterAddCacheChanged)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(1,
                                                          defName,
                                                          TimePointFromString("2022-11-11 15:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    alarmOperations->addAlarm(AlarmEventRecord(2,
                                               defName,
                                               TimePointFromString("2022-11-11 11:00:00"),
                                               defDuration,
                                               defAllDay,
                                               defRRule,
                                               defMusic,
                                               defEnabled,
                                               defSnooze),
                              universalBoolCallback);
    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 11:00:00"));
}

TEST_F(AlarmOperationsFixture, handleEventAfterAddCacheNotChanged)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(1,
                                                          defName,
                                                          TimePointFromString("2022-11-11 15:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    alarmOperations->addAlarm(AlarmEventRecord(2,
                                               defName,
                                               TimePointFromString("2022-11-11 17:00:00"),
                                               defDuration,
                                               defAllDay,
                                               defRRule,
                                               defMusic,
                                               defEnabled,
                                               defSnooze),
                              universalBoolCallback);
    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 15:00:00"));
}

TEST_F(AlarmOperationsFixture, handleEventAfterAddCacheNotChanged2)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(1,
                                                          defName,
                                                          TimePointFromString("2022-11-11 15:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    alarmOperations->addAlarm(AlarmEventRecord(2,
                                               defName,
                                               TimePointFromString("2022-11-10 12:00:00"),
                                               defDuration,
                                               defAllDay,
                                               defRRule,
                                               defMusic,
                                               defEnabled,
                                               defSnooze),
                              universalBoolCallback);
    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 15:00:00"));
}

TEST_F(AlarmOperationsFixture, handleEventAfterAddDisabled)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(1,
                                                          defName,
                                                          TimePointFromString("2022-11-11 15:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    alarmOperations->addAlarm(AlarmEventRecord(2,
                                               defName,
                                               TimePointFromString("2022-11-11 11:00:00"),
                                               defDuration,
                                               defAllDay,
                                               defRRule,
                                               defMusic,
                                               false,
                                               defSnooze),
                              universalBoolCallback);
    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 15:00:00"));
}

TEST_F(AlarmOperationsFixture, handleAfterRemoveNearest)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(1,
                                                          defName,
                                                          TimePointFromString("2022-11-11 11:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(2,
                                                          defName,
                                                          TimePointFromString("2022-11-11 09:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));

    alarmOperations->removeAlarm(2, universalBoolCallback);

    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 11:00:00"));
}

TEST_F(AlarmOperationsFixture, handleAfterRemoveNotNearest)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(1,
                                                          defName,
                                                          TimePointFromString("2022-11-11 11:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(2,
                                                          defName,
                                                          TimePointFromString("2022-11-11 09:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));

    alarmOperations->removeAlarm(1, universalBoolCallback);

    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 09:00:00"));
}

TEST_F(AlarmOperationsFixture, handleAfterUpdateNearestDelayed)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(1,
                                                          defName,
                                                          TimePointFromString("2022-11-11 11:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));
    auto nearestAlarm = AlarmEventRecord(2,
                                         defName,
                                         TimePointFromString("2022-11-11 09:00:00"),
                                         defDuration,
                                         defAllDay,
                                         defRRule,
                                         defMusic,
                                         defEnabled,
                                         defSnooze);
    alarmRepoMock->nextRecords.push_back(nearestAlarm);

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));

    nearestAlarm.startDate = TimePointFromString("2022-11-11 15:00:00");
    alarmOperations->updateAlarm(nearestAlarm, universalBoolCallback);

    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 11:00:00"));
}

TEST_F(AlarmOperationsFixture, handleAfterUpdateSecondGetsFirst)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    auto secondAlarm   = AlarmEventRecord(1,
                                        defName,
                                        TimePointFromString("2022-11-11 11:00:00"),
                                        defDuration,
                                        defAllDay,
                                        defRRule,
                                        defMusic,
                                        defEnabled,
                                        defSnooze);
    alarmRepoMock->nextRecords.push_back(secondAlarm);

    alarmRepoMock->nextRecords.push_back(AlarmEventRecord(2,
                                                          defName,
                                                          TimePointFromString("2022-11-11 09:00:00"),
                                                          defDuration,
                                                          defAllDay,
                                                          defRRule,
                                                          defMusic,
                                                          defEnabled,
                                                          defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));

    secondAlarm.startDate = TimePointFromString("2022-11-11 7:00:00");
    alarmOperations->updateAlarm(secondAlarm, universalBoolCallback);

    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 7:00:00"));
}

TEST_F(AlarmOperationsFixture, snoozeCountChangeCallback)
{
    auto alarmRepoMock   = std::make_unique<MockAlarmEventsRepository>();
    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    int i         = 1;
    auto callback = [&](unsigned count) { i = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(callback);
    alarmOperations->stopAllSnoozedAlarms();
    EXPECT_EQ(i, 0);
}
