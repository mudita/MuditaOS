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

alarms::IAlarmOperations::GetCurrentTime timeInjector = []() {
    return TimePointFromStringWithShift("2022-11-11 05:00:00");
};
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

constexpr auto defRRule    = "";
constexpr auto defMusic    = "";
constexpr auto defEnabled  = true;
constexpr auto defSnooze   = 15;

TEST_F(AlarmOperationsFixture, getEnabledEvents)
{
    alarms::IAlarmOperations::OnGetAlarmsProcessed callback = [](std::vector<SingleEventRecord> records) {
        EXPECT_EQ(records.size(), 2);
        EXPECT_EQ(records[0].parent->ID, 1);
        EXPECT_EQ(records[1].parent->ID, 3);
    };

    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{9h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(2, AlarmTime{9h, 0min}, defMusic, false, defSnooze, defRRule));
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(3, AlarmTime{9h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->getNextSingleEvents(TimePointFromStringWithShift("2022-11-11 00:00:00"), callback);
}

TEST_F(AlarmOperationsFixture, handleFirstEvent)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{9h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(2, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 08:59:00"));
    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 09:00:00"));
}

TEST_F(AlarmOperationsFixture, handleEventAfterAddCacheChanged)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{15h, 0min}, defMusic, defEnabled, defSnooze, defRRule));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));
    alarmOperations->addAlarm(AlarmEventRecord(2, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule),
                              universalBoolCallback);
    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 11:00:00"));
}

TEST_F(AlarmOperationsFixture, handleEventAfterAddCacheNotChanged)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{15h, 0min}, defMusic, defEnabled, defSnooze, defRRule));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));
    alarmOperations->addAlarm(AlarmEventRecord(2, AlarmTime{17h, 0min}, defMusic, defEnabled, defSnooze, defRRule),
                              universalBoolCallback);
    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 15:00:00"));
}

TEST_F(AlarmOperationsFixture, handleEventAfterAddCacheNotChanged2)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{15h, 0min}, defMusic, defEnabled, defSnooze, defRRule));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));
    alarmOperations->addAlarm(AlarmEventRecord(2, AlarmTime{12h, 0min}, defMusic, defEnabled, defSnooze, defRRule),
                              universalBoolCallback);
    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 15:00:00"));
}

TEST_F(AlarmOperationsFixture, handleEventAfterAddDisabled)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{15h, 0min}, defMusic, defEnabled, defSnooze, defRRule));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));
    alarmOperations->addAlarm(AlarmEventRecord(2, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule),
                              universalBoolCallback);
    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 15:00:00"));
}

TEST_F(AlarmOperationsFixture, handleAfterRemoveNearest)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(2, AlarmTime{9h, 0min}, defMusic, defEnabled, defSnooze, defRRule));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));

    alarmOperations->removeAlarm(2, universalBoolCallback);

    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 11:00:00"));
}

TEST_F(AlarmOperationsFixture, handleAfterRemoveNotNearest)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(2, AlarmTime{9h, 0min}, defMusic, defEnabled, defSnooze, defRRule));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));

    alarmOperations->removeAlarm(1, universalBoolCallback);

    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 09:00:00"));
}

TEST_F(AlarmOperationsFixture, handleAfterUpdateNearestDelayed)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    auto nearestAlarm = AlarmEventRecord(2, AlarmTime{9h, 0min}, defMusic, defEnabled, defSnooze, defRRule);
    alarmRepoMock->nextRecords.push_back(nearestAlarm);

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));

    nearestAlarm.alarmTime = AlarmTime{15h, 0min};
    alarmOperations->updateAlarm(nearestAlarm, universalBoolCallback);

    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 11:00:00"));
}

TEST_F(AlarmOperationsFixture, handleAfterUpdateSecondGetsFirst)
{
    auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    auto secondAlarm = AlarmEventRecord(2, AlarmTime{9h, 0min}, defMusic, defEnabled, defSnooze, defRRule);
    alarmRepoMock->nextRecords.push_back(secondAlarm);

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));

    secondAlarm.alarmTime = AlarmTime{7h, 0min};
    alarmOperations->updateAlarm(secondAlarm, universalBoolCallback);

    auto handler = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(1);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 7:00:00"));
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

    alarmOperations->addAlarm(AlarmEventRecord(1, AlarmTime{17h, 0min}, defMusic, defEnabled, defSnooze, defRRule),
                              universalBoolCallback);

    alarmOperations->addAlarm(AlarmEventRecord(2, AlarmTime{17h, 0min}, defMusic, defEnabled, defSnooze, defRRule),
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
    const auto alarm1time = TimePointFromStringWithShift("2022-11-11 11:00:00");
    const auto alarm2time = TimePointFromStringWithShift("2022-11-11 11:02:00");
    const auto alarm3time = TimePointFromStringWithShift("2022-11-11 11:04:00");
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(2, AlarmTime{11h, 2min}, defMusic, defEnabled, defSnooze, defRRule));
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(3, AlarmTime{11h, 4min}, defMusic, defEnabled, defSnooze, defRRule));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    int snoozedCount   = 0;
    auto countcallback = [&](unsigned count) { snoozedCount = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(countcallback);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    EXPECT_EQ(snoozedCount, 0);

    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));
    EXPECT_EQ(alarmActive, true);

    alarmOperations->minuteUpdated(alarm1time);
    alarmOperations->snoozeRingingAlarm(1, TimePointFromStringWithShift("2022-11-11 11:10:00"), universalBoolCallback);
    EXPECT_EQ(snoozedCount, 1);

    alarmOperations->minuteUpdated(alarm2time);
    alarmOperations->snoozeRingingAlarm(2, TimePointFromStringWithShift("2022-11-11 11:08:00"), universalBoolCallback);
    EXPECT_EQ(snoozedCount, 2);

    alarmOperations->minuteUpdated(alarm3time);
    alarmOperations->snoozeRingingAlarm(3, TimePointFromStringWithShift("2022-11-11 11:09:00"), universalBoolCallback);
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
    const auto alarmTime = TimePointFromStringWithShift("2022-11-11 11:00:00");
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    int snoozedCount   = 0;
    auto countcallback = [&](unsigned count) { snoozedCount = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(countcallback);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    EXPECT_EQ(snoozedCount, 0);

    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));
    EXPECT_TRUE(alarmActive);

    const auto snoozeTime = TimePointFromStringWithShift("2022-11-11 11:10:00");
    alarmOperations->minuteUpdated(alarmTime);
    alarmOperations->snoozeRingingAlarm(1, snoozeTime, [](bool) {});
    EXPECT_EQ(snoozedCount, 1);

    std::vector<SingleEventRecord> snoozedEvents;
    auto cb = [&](std::vector<SingleEventRecord> records) { snoozedEvents = std::move(records); };
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_EQ(snoozedEvents.size(), 1);
    EXPECT_EQ(snoozedEvents.front().startDate, snoozeTime);

    const auto postponedSnoozeTime = TimePointFromStringWithShift("2022-11-11 11:20:00");
    alarmOperations->postponeSnooze(snoozedEvents.front().parent->ID, postponedSnoozeTime, [](bool) {});
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_EQ(snoozedEvents.size(), 1);
    EXPECT_EQ(snoozedEvents.front().startDate, postponedSnoozeTime);
}

TEST_F(AlarmOperationsFixture, snoozeMultipleTimes)
{
    auto alarmRepoMock   = std::make_unique<MockAlarmEventsRepository>();
    const auto alarmTime = TimePointFromStringWithShift("2022-11-11 11:00:00");
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    int snoozedCount   = 0;
    auto countcallback = [&](unsigned count) { snoozedCount = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(countcallback);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    EXPECT_EQ(snoozedCount, 0);

    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));
    EXPECT_TRUE(alarmActive);

    const auto snoozeTime = TimePointFromStringWithShift("2022-11-11 11:10:00");
    alarmOperations->minuteUpdated(alarmTime);
    alarmOperations->snoozeRingingAlarm(1, snoozeTime, [](bool) {});
    EXPECT_EQ(snoozedCount, 1);

    alarmOperations->updateEventsCache(snoozeTime);
    std::vector<SingleEventRecord> snoozedEvents;
    auto cb = [&](std::vector<SingleEventRecord> records) { snoozedEvents = std::move(records); };
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_EQ(snoozedEvents.size(), 1);
    EXPECT_EQ(snoozedEvents.front().startDate, snoozeTime);

    const auto anotherSnoozeTime = TimePointFromStringWithShift("2022-11-11 11:20:00");
    alarmOperations->snoozeRingingAlarm(snoozedEvents.front().parent->ID, anotherSnoozeTime, [](bool) {});
    alarmOperations->getSnoozedAlarms(cb);
    EXPECT_EQ(snoozedEvents.size(), 1);
}

TEST_F(AlarmOperationsFixture, stopSnooze)
{
    auto alarmRepoMock   = std::make_unique<MockAlarmEventsRepository>();
    const auto alarmTime = TimePointFromStringWithShift("2022-11-11 11:00:00");
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    int snoozedCount   = 0;
    auto countcallback = [&](unsigned count) { snoozedCount = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(countcallback);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    EXPECT_EQ(snoozedCount, 0);

    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));
    EXPECT_TRUE(alarmActive);

    const auto snoozeTime = TimePointFromStringWithShift("2022-11-11 11:10:00");
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
}

TEST_F(AlarmOperationsFixture, handleMultipleAlarmsWithSnooze)
{
    auto alarmRepoMock          = std::make_unique<MockAlarmEventsRepository>();
    const auto alarm1time       = TimePointFromStringWithShift("2022-11-11 11:00:00");
    const auto alarm2time       = TimePointFromStringWithShift("2022-11-11 11:02:00");
    const auto alarm1timeSnooze = TimePointFromStringWithShift("2022-11-11 11:10:00");
    const auto alarm2timeSnooze = TimePointFromStringWithShift("2022-11-11 11:06:00");
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(2, AlarmTime{11h, 2min}, defMusic, defEnabled, defSnooze, defRRule));

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

    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 08:59:00"));

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
}

TEST_F(AlarmOperationsFixture, recurrentAlarmHandling)
{
    auto alarmRepoMock    = std::make_unique<MockAlarmEventsRepository>();
    const auto alarmTime  = TimePointFromStringWithShift("2022-11-11 11:00:00");
    const auto alarmRrule = "FREQ=WEEKLY;BYDAY=MO,TU,WE,TH,FR,SA,SU";
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, alarmRrule));

    constexpr auto daysInWeek = 7;

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    auto handler         = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(daysInWeek);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));
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
    const auto alarmRrule      = "FREQ=WEEKLY;BYDAY=MO,TU,WE,TH,FR,SA,SU";
    constexpr auto defDisabled = false;
    AlarmEventRecord alarmRecord{1, AlarmTime{11h, 0min}, defMusic, defDisabled, defSnooze, alarmRrule};
    alarmRepoMock->addAlarmEvent(alarmRecord, universalBoolCallback);

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);

    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 09:00:00"));
    EXPECT_FALSE(alarmActive);

    alarmRecord.enabled = defEnabled;
    alarmOperations->updateAlarm(alarmRecord, universalBoolCallback);
    EXPECT_TRUE(alarmActive);

    alarmRecord.enabled = defDisabled;
    alarmOperations->updateAlarm(alarmRecord, universalBoolCallback);
    EXPECT_FALSE(alarmActive);
}

TEST_F(AlarmOperationsFixture, handleMultipleConcurrentAlarms)
{
    auto alarmRepoMock         = std::make_unique<MockAlarmEventsRepository>();
    const auto alarmTime       = TimePointFromStringWithShift("2022-11-11 11:00:00");
    const auto alarmTimeSnooze = TimePointFromStringWithShift("2022-11-11 11:10:00");
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(1, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(2, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));
    alarmRepoMock->nextRecords.push_back(
        AlarmEventRecord(3, AlarmTime{11h, 0min}, defMusic, defEnabled, defSnooze, defRRule));

    auto alarmOperations = getMockedAlarmOperations(alarmRepoMock);
    auto handler         = std::make_shared<MockAlarmHandler>();
    EXPECT_CALL(*handler, handle(testing::_)).Times(6);
    alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock, handler);

    int snoozedCount   = 0;
    auto countcallback = [&](unsigned count) { snoozedCount = count; };
    alarmOperations->addSnoozedAlarmsCountChangeCallback(countcallback);
    bool alarmActive    = false;
    auto activeCallback = [&](bool isAnyActive) { alarmActive = isAnyActive; };
    alarmOperations->addActiveAlarmCountChangeCallback(activeCallback);

    alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 08:59:00"));

    alarmOperations->minuteUpdated(alarmTime);
    alarmOperations->snoozeRingingAlarm(1, alarmTimeSnooze, universalBoolCallback);
    alarmOperations->snoozeRingingAlarm(2, alarmTimeSnooze, universalBoolCallback);
    alarmOperations->snoozeRingingAlarm(3, alarmTimeSnooze, universalBoolCallback);
    EXPECT_EQ(snoozedCount, 3);
    EXPECT_TRUE(alarmActive);

    alarmOperations->minuteUpdated(alarmTimeSnooze);
    EXPECT_EQ(snoozedCount, 0);
    alarmOperations->turnOffRingingAlarm(1, universalBoolCallback);
    alarmOperations->turnOffRingingAlarm(2, universalBoolCallback);
    alarmOperations->turnOffRingingAlarm(3, universalBoolCallback);
}
