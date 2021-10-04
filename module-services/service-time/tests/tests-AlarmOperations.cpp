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
    MOCK_METHOD(void, toggleAll, (const bool toggle, const alarms::OnToggleAll &callback), ());

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
        std::vector<AlarmEventRecord> result;
        for (const auto &rec : recurringRecords) {
            if (rec.enabled && rec.startDate <= end && rec.endDate >= start) {
                result.push_back(rec);
            }
        }
        callback({result});
    }

    void getNext(TimePoint start, std::uint32_t offset, std::uint32_t limit, const alarms::OnGetNextCallback &callback)
    {
        std::vector<AlarmEventRecord> result;
        for (const auto &rec : nextRecords) {
            if (rec.enabled && rec.startDate > start) {
                result.push_back(rec);
            }
        }
        callback({result});
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
    std::unique_ptr<alarms::IAlarmOperations> getMockedAlarmOperations(
        std::unique_ptr<MockAlarmEventsRepository> &alarmRepo);
};

#if defined COMMON_ALARM_OPERATIONS_TEST
std::unique_ptr<alarms::IAlarmOperations> AlarmOperationsFixture::getMockedAlarmOperations(
    std::unique_ptr<MockAlarmEventsRepository> &alarmRepo)
{
    return std::make_unique<alarms::AlarmOperationsCommon>(std::move(alarmRepo), timeInjector);
}
#endif

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
    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 08:59:00"));
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

TEST_F(AlarmOperationsFixture, toggleAll)
{
    alarms::IAlarmOperations::OnGetAlarmProcessed callbackOn = [](AlarmEventRecord record) {
        EXPECT_TRUE(record.enabled);
    };

    alarms::IAlarmOperations::OnGetAlarmProcessed callbackOff = [](AlarmEventRecord record) {
        EXPECT_FALSE(record.enabled);
    };

    auto alarmRepoMock   = std::make_unique<MockAlarmEventsRepository>();
    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    alarmOperations->addAlarm(AlarmEventRecord(1,
                                               defName,
                                               TimePointFromString("2022-11-11 17:00:00"),
                                               defDuration,
                                               defAllDay,
                                               defRRule,
                                               defMusic,
                                               false,
                                               defSnooze),
                              universalBoolCallback);

    alarmOperations->addAlarm(AlarmEventRecord(2,
                                               defName,
                                               TimePointFromString("2022-11-11 17:00:00"),
                                               defDuration,
                                               defAllDay,
                                               defRRule,
                                               defMusic,
                                               true,
                                               defSnooze),
                              universalBoolCallback);

    alarmOperations->getAlarm(1, callbackOff);
    alarmOperations->getAlarm(2, callbackOn);

    alarmOperations->toggleAll(true, universalBoolCallback);
    alarmOperations->getAlarm(1, callbackOn);
    alarmOperations->getAlarm(2, callbackOn);

    alarmOperations->toggleAll(false, universalBoolCallback);
    alarmOperations->getAlarm(1, callbackOff);
    alarmOperations->getAlarm(2, callbackOff);
}

TEST_F(AlarmOperationsFixture, getSnoozedAlarmsList)
{
    auto alarmRepoMock    = std::make_unique<MockAlarmEventsRepository>();
    const auto alarm1time = TimePointFromString("2022-11-11 11:00:00");
    const auto alarm2time = TimePointFromString("2022-11-11 11:02:00");
    const auto alarm3time = TimePointFromString("2022-11-11 11:04:00");
    alarmRepoMock->addSingleEvent(
        AlarmEventRecord(1, defName, alarm1time, defDuration, defAllDay, defRRule, defMusic, defEnabled, defSnooze));
    alarmRepoMock->addSingleEvent(
        AlarmEventRecord(2, defName, alarm2time, defDuration, defAllDay, defRRule, defMusic, defEnabled, defSnooze));
    alarmRepoMock->addSingleEvent(
        AlarmEventRecord(3, defName, alarm3time, defDuration, defAllDay, defRRule, defMusic, defEnabled, defSnooze));
    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    int snoozedCount   = 0;
    auto countcallback = [&](unsigned count) { snoozedCount = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(countcallback);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    EXPECT_EQ(snoozedCount, 0);

    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    EXPECT_EQ(alarmActive, true);

    alarmOperations->minuteUpdated(alarm1time);
    alarmOperations->snoozeRingingAlarm(1, TimePointFromString("2022-11-11 11:10:00"), universalBoolCallback);
    EXPECT_EQ(snoozedCount, 1);

    alarmOperations->minuteUpdated(alarm2time);
    alarmOperations->snoozeRingingAlarm(2, TimePointFromString("2022-11-11 11:08:00"), universalBoolCallback);
    EXPECT_EQ(snoozedCount, 2);

    alarmOperations->minuteUpdated(alarm3time);
    alarmOperations->snoozeRingingAlarm(3, TimePointFromString("2022-11-11 11:09:00"), universalBoolCallback);
    EXPECT_EQ(snoozedCount, 3);

    std::vector<SingleEventRecord> snoozedEvents;
    auto cb = [&](std::vector<SingleEventRecord> records) { snoozedEvents = std::move(records); };
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_EQ(snoozedCount, 3);
    EXPECT_EQ(snoozedEvents.size(), 3);
    // Check if events are sorted starting from nearest
    EXPECT_EQ(snoozedEvents.at(0).parent->ID, 2);
    EXPECT_EQ(snoozedEvents.at(1).parent->ID, 3);
    EXPECT_EQ(snoozedEvents.at(2).parent->ID, 1);
    EXPECT_EQ(snoozedEvents.front().parent->ID, 2);
}

TEST_F(AlarmOperationsFixture, postponeSnooze)
{
    auto alarmRepoMock   = std::make_unique<MockAlarmEventsRepository>();
    const auto alarmTime = TimePointFromString("2022-11-11 11:00:00");
    alarmRepoMock->addSingleEvent(
        AlarmEventRecord(1, defName, alarmTime, defDuration, defAllDay, defRRule, defMusic, defEnabled, defSnooze));
    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    int snoozedCount   = 0;
    auto countcallback = [&](unsigned count) { snoozedCount = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(countcallback);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    EXPECT_EQ(snoozedCount, 0);

    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    EXPECT_TRUE(alarmActive);

    const auto snoozeTime = TimePointFromString("2022-11-11 11:10:00");
    alarmOperations->minuteUpdated(alarmTime);
    alarmOperations->snoozeRingingAlarm(1, snoozeTime, [](bool) {});
    EXPECT_EQ(snoozedCount, 1);

    std::vector<SingleEventRecord> snoozedEvents;
    auto cb = [&](std::vector<SingleEventRecord> records) { snoozedEvents = std::move(records); };
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_EQ(snoozedEvents.size(), 1);
    EXPECT_EQ(snoozedEvents.front().startDate, snoozeTime);

    const auto postponedSnoozeTime = TimePointFromString("2022-11-11 11:20:00");
    alarmOperations->postponeSnooze(snoozedEvents.front().parent->ID, postponedSnoozeTime, [](bool) {});
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_EQ(snoozedEvents.size(), 1);
    EXPECT_EQ(snoozedEvents.front().startDate, postponedSnoozeTime);
}

TEST_F(AlarmOperationsFixture, snoozeMultipleTimes)
{
    auto alarmRepoMock   = std::make_unique<MockAlarmEventsRepository>();
    const auto alarmTime = TimePointFromString("2022-11-11 11:00:00");
    alarmRepoMock->addSingleEvent(
        AlarmEventRecord(1, defName, alarmTime, defDuration, defAllDay, defRRule, defMusic, defEnabled, defSnooze));
    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    int snoozedCount   = 0;
    auto countcallback = [&](unsigned count) { snoozedCount = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(countcallback);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    EXPECT_EQ(snoozedCount, 0);

    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    EXPECT_TRUE(alarmActive);

    const auto snoozeTime = TimePointFromString("2022-11-11 11:10:00");
    alarmOperations->minuteUpdated(alarmTime);
    alarmOperations->snoozeRingingAlarm(1, snoozeTime, [](bool) {});
    EXPECT_EQ(snoozedCount, 1);

    alarmOperations->updateEventsCache(snoozeTime);
    std::vector<SingleEventRecord> snoozedEvents;
    auto cb = [&](std::vector<SingleEventRecord> records) { snoozedEvents = std::move(records); };
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_EQ(snoozedEvents.size(), 1);
    EXPECT_EQ(snoozedEvents.front().startDate, snoozeTime);

    const auto anotherSnoozeTime = TimePointFromString("2022-11-11 11:20:00");
    alarmOperations->snoozeRingingAlarm(snoozedEvents.front().parent->ID, anotherSnoozeTime, [](bool) {});
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_EQ(snoozedEvents.size(), 1);
}

TEST_F(AlarmOperationsFixture, stopSnooze)
{
    auto alarmRepoMock   = std::make_unique<MockAlarmEventsRepository>();
    const auto alarmTime = TimePointFromString("2022-11-11 11:00:00");
    alarmRepoMock->addSingleEvent(
        AlarmEventRecord(1, defName, alarmTime, defDuration, defAllDay, defRRule, defMusic, defEnabled, defSnooze));
    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    int snoozedCount   = 0;
    auto countcallback = [&](unsigned count) { snoozedCount = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(countcallback);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    EXPECT_EQ(snoozedCount, 0);

    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    EXPECT_TRUE(alarmActive);

    const auto snoozeTime = TimePointFromString("2022-11-11 11:10:00");
    alarmOperations->minuteUpdated(alarmTime);
    alarmOperations->snoozeRingingAlarm(1, snoozeTime, [](bool) {});
    EXPECT_EQ(snoozedCount, 1);
    alarmOperations->updateEventsCache(snoozeTime);

    std::vector<SingleEventRecord> snoozedEvents;
    auto cb = [&](std::vector<SingleEventRecord> records) { snoozedEvents = std::move(records); };
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_EQ(snoozedEvents.size(), 1);
    EXPECT_EQ(snoozedEvents.front().startDate, snoozeTime);

    alarmOperations->turnOffSnoozedAlarm(snoozedEvents.front().parent->ID, [](bool) {});
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_TRUE(snoozedEvents.empty());
    EXPECT_EQ(snoozedCount, 0);
    EXPECT_FALSE(alarmActive);
}

TEST_F(AlarmOperationsFixture, handleMultipleAlarmsWithSnooze)
{
    auto alarmRepoMock          = std::make_unique<MockAlarmEventsRepository>();
    const auto alarm1time       = TimePointFromString("2022-11-11 11:00:00");
    const auto alarm2time       = TimePointFromString("2022-11-11 11:02:00");
    const auto alarm1timeSnooze = TimePointFromString("2022-11-11 11:10:00");
    const auto alarm2timeSnooze = TimePointFromString("2022-11-11 11:06:00");
    alarmRepoMock->addSingleEvent(
        AlarmEventRecord(1, defName, alarm1time, defDuration, defAllDay, defRRule, defMusic, defEnabled, defSnooze));
    alarmRepoMock->addSingleEvent(
        AlarmEventRecord(2, defName, alarm2time, defDuration, defAllDay, defRRule, defMusic, defEnabled, defSnooze));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    auto handler         = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(4);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);

    int snoozedCount   = 0;
    auto countcallback = [&](unsigned count) { snoozedCount = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(countcallback);
    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 08:59:00"));

    alarmOperations->minuteUpdated(alarm1time);
    alarmOperations->snoozeRingingAlarm(1, alarm1timeSnooze, universalBoolCallback);
    EXPECT_EQ(snoozedCount, 1);
    EXPECT_TRUE(alarmActive);

    alarmOperations->minuteUpdated(alarm2time);
    alarmOperations->snoozeRingingAlarm(2, alarm2timeSnooze, universalBoolCallback);
    EXPECT_EQ(snoozedCount, 2);
    EXPECT_TRUE(alarmActive);

    alarmOperations->minuteUpdated(alarm2timeSnooze);
    alarmOperations->turnOffRingingAlarm(2, universalBoolCallback);
    EXPECT_EQ(snoozedCount, 1);
    EXPECT_TRUE(alarmActive);

    alarmOperations->minuteUpdated(alarm1timeSnooze);
    alarmOperations->turnOffRingingAlarm(1, universalBoolCallback);
    EXPECT_EQ(snoozedCount, 0);
    EXPECT_FALSE(alarmActive);
}

TEST_F(AlarmOperationsFixture, recurrentAlarmHandling)
{
    auto alarmRepoMock    = std::make_unique<MockAlarmEventsRepository>();
    const auto alarmTime  = TimePointFromString("2022-11-11 11:00:00");
    const auto alarmRrule = "FREQ=WEEKLY;BYDAY=MO,TU,WE,TH,FR,SA,SU";
    alarmRepoMock->addRecurringEvent(
        AlarmEventRecord(1, defName, alarmTime, defDuration, defAllDay, alarmRrule, defMusic, defEnabled, defSnooze));

    constexpr auto daysInWeek = 7;

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    auto handler         = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(daysInWeek);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    EXPECT_TRUE(alarmActive);

    for (auto i = 0; i < daysInWeek; ++i) {
        alarmOperations->minuteUpdated(alarmTime + date::days{i});
        alarmOperations->turnOffRingingAlarm(1, universalBoolCallback);
        EXPECT_TRUE(alarmActive);
    }
}

TEST_F(AlarmOperationsFixture, recurrentAlarmEnablingDisabling)
{
    auto alarmRepoMock         = std::make_unique<MockAlarmEventsRepository>();
    const auto alarmTime       = TimePointFromString("2022-11-11 11:00:00");
    const auto alarmRrule      = "FREQ=WEEKLY;BYDAY=MO,TU,WE,TH,FR,SA,SU";
    constexpr auto defDisabled = false;
    AlarmEventRecord alarmRecord{
        1, defName, alarmTime, defDuration, defAllDay, alarmRrule, defMusic, defDisabled, defSnooze};
    alarmRepoMock->addRecurringEvent(alarmRecord);

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 09:00:00"));
    EXPECT_FALSE(alarmActive);

    alarmRecord.enabled = defEnabled;
    alarmOperations->updateAlarm(alarmRecord, universalBoolCallback);
    EXPECT_TRUE(alarmActive);

    alarmRecord.enabled = defDisabled;
    alarmOperations->updateAlarm(alarmRecord, universalBoolCallback);
    EXPECT_FALSE(alarmActive);
}
