// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <AlarmOperations.hpp>
#include <common/models/BedtimeModel.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>

#include <products/BellHybrid/services/time/include/time/AlarmOperations.hpp>

#include <gtest/gtest.h>

// Included a .cpp file with AlarmOperations tests for the common part (the base class' implementation).
// It needs to be built into this test in order to test the possible regression introduced by
// the Bell-specific AlarmOperations class.
// Generally, this should not be solved this way in other cases.
#include <service-time/tests/tests-AlarmOperations.cpp>

namespace
{
    class MockedPreWakeUpSettingsProvider : public alarms::PreWakeUpSettingsProvider
    {
      public:
        void setChimeSettings(bool enabled, std::chrono::minutes minutes)
        {
            chimeSettings = Settings{enabled, minutes};
        }

        void setFrontlightSettings(bool enabled, std::chrono::minutes minutes)
        {
            frontlightSettings = Settings{enabled, minutes};
        }

        auto getChimeSettings() -> Settings override
        {
            return chimeSettings;
        }

        auto getFrontlightSettings() -> Settings override
        {
            return frontlightSettings;
        }

      private:
        Settings chimeSettings{true, std::chrono::minutes{5}};
        Settings frontlightSettings{true, std::chrono::minutes{2}};
    };

    class MockedSnoozeChimeSettingsProvider : public alarms::SnoozeChimeSettingsProvider
    {
      public:
        void setSettings(bool enabled, std::chrono::minutes interval)
        {
            settings = Settings{enabled, interval};
        }

        auto getSettings() -> Settings override
        {
            return settings;
        }

      private:
        Settings settings{true, std::chrono::minutes{1}};
    };

    class MockedBedtimeModel : public alarms::AbstractBedtimeSettingsProvider
    {
      public:
        MockedBedtimeModel() : bedtimeOnOff{false}, bedtimeTime{0}
        {}
        MockedBedtimeModel(bool OnOff, TimePoint time) : bedtimeOnOff{OnOff}, bedtimeTime{TimePointToTimeT(time)}
        {}
        auto isBedtimeEnabled() -> bool override
        {
            return bedtimeOnOff;
        }
        auto getBedtimeTime() -> time_t override
        {
            return bedtimeTime;
        }

      private:
        bool bedtimeOnOff;
        time_t bedtimeTime;
    };

    std::unique_ptr<alarms::IAlarmOperations> getMockedAlarmOperations(
        std::unique_ptr<MockAlarmEventsRepository> &alarmRepo,
        std::unique_ptr<alarms::PreWakeUpSettingsProvider> &&preWakeUpSettingsProvider,
        std::unique_ptr<alarms::SnoozeChimeSettingsProvider> &&snoozeChimeSettingsProvider,
        std::unique_ptr<alarms::AbstractBedtimeSettingsProvider> &&bedtimeSettingsProvider)
    {
        return std::make_unique<alarms::AlarmOperations>(std::move(alarmRepo),
                                                         timeInjector,
                                                         std::move(preWakeUpSettingsProvider),
                                                         std::move(snoozeChimeSettingsProvider),
                                                         std::move(bedtimeSettingsProvider));
    }
} // namespace

// This function replaces the `getMockedAlarmOperations` function from `service-time/tests/tests-AlarmOperations.cpp`
std::unique_ptr<alarms::IAlarmOperations> AlarmOperationsFixture::getMockedAlarmOperations(
    std::unique_ptr<MockAlarmEventsRepository> &alarmRepo)
{
    return ::getMockedAlarmOperations(alarmRepo,
                                      std::make_unique<MockedPreWakeUpSettingsProvider>(),
                                      std::make_unique<MockedSnoozeChimeSettingsProvider>(),
                                      std::make_unique<MockedBedtimeModel>());
}

TEST(PreWakeUp, TooEarlyForPreWakeUp)
{
    alarms::PreWakeUp preWakeUp(std::make_unique<MockedPreWakeUpSettingsProvider>());
    auto event           = AlarmEventRecord(1, AlarmTime{5h, 6min}, defMusic, defEnabled, defSnooze, defRRule);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromStringWithShift("2022-11-11 05:00:00"));
    EXPECT_TRUE(static_cast<EventInfo>(nextEvent).isValid());

    const auto decision = preWakeUp.decide(timeInjector(), nextEvent);
    ASSERT_FALSE(decision.timeForChime);
    ASSERT_FALSE(decision.timeForFrontlight);
}

TEST(PreWakeUp, TimeToPlayChime)
{
    alarms::PreWakeUp preWakeUp(std::make_unique<MockedPreWakeUpSettingsProvider>());
    auto event           = AlarmEventRecord(1, AlarmTime{5h, 5min}, defMusic, defEnabled, defSnooze, defRRule);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromStringWithShift("2022-11-11 05:00:00"));
    EXPECT_TRUE(static_cast<EventInfo>(nextEvent).isValid());

    const auto decision = preWakeUp.decide(timeInjector(), nextEvent);
    ASSERT_TRUE(decision.timeForChime);
    ASSERT_FALSE(decision.timeForFrontlight);
}

TEST(PreWakeUp, TimeToPlayChimeButDisabled)
{
    auto settingsProvider = std::make_unique<MockedPreWakeUpSettingsProvider>();
    settingsProvider->setChimeSettings(false, std::chrono::minutes::zero());
    alarms::PreWakeUp preWakeUp(std::move(settingsProvider));
    auto event           = AlarmEventRecord(1, AlarmTime{5h, 5min}, defMusic, defEnabled, defSnooze, defRRule);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromStringWithShift("2022-11-11 05:00:00"));
    EXPECT_TRUE(static_cast<EventInfo>(nextEvent).isValid());

    const auto decision = preWakeUp.decide(timeInjector(), nextEvent);
    ASSERT_FALSE(decision.timeForChime);
    ASSERT_FALSE(decision.timeForFrontlight);
}

TEST(PreWakeUp, TimeToLightFrontlight)
{
    alarms::PreWakeUp preWakeUp(std::make_unique<MockedPreWakeUpSettingsProvider>());
    auto event           = AlarmEventRecord(1, AlarmTime{5h, 2min}, defMusic, defEnabled, defSnooze, defRRule);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromStringWithShift("2022-11-11 05:00:00"));
    EXPECT_TRUE(static_cast<EventInfo>(nextEvent).isValid());

    const auto decision = preWakeUp.decide(timeInjector(), nextEvent);
    ASSERT_FALSE(decision.timeForChime);
    ASSERT_TRUE(decision.timeForFrontlight);
}

TEST(PreWakeUp, TimeToLightFrontlightButDisabled)
{
    auto settingsProvider = std::make_unique<MockedPreWakeUpSettingsProvider>();
    settingsProvider->setFrontlightSettings(false, std::chrono::minutes::zero());
    alarms::PreWakeUp preWakeUp(std::move(settingsProvider));
    auto event           = AlarmEventRecord(1, AlarmTime{5h, 2min}, defMusic, defEnabled, defSnooze, defRRule);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromStringWithShift("2022-11-11 05:00:00"));
    EXPECT_TRUE(static_cast<EventInfo>(nextEvent).isValid());

    const auto decision = preWakeUp.decide(timeInjector(), nextEvent);
    ASSERT_FALSE(decision.timeForChime);
    ASSERT_FALSE(decision.timeForFrontlight);
}

TEST(Bedtime, TimeToBedDisabled)
{
    auto settingsProvider =
        std::make_unique<MockedBedtimeModel>(false, TimePointFromStringWithShift("2022-11-11 05:02:00"));
    alarms::Bedtime bedtime(std::move(settingsProvider));
    ASSERT_FALSE(bedtime.decide(TimePointFromStringWithShift("2022-11-11 05:02:00")));
    ASSERT_FALSE(bedtime.decide(TimePointFromStringWithShift("2022-11-11 07:32:00")));
}

TEST(Bedtime, TimeToBedEnabled)
{
    auto settingsProvider =
        std::make_unique<MockedBedtimeModel>(true, TimePointFromStringWithShift("2022-11-11 05:02:00"));
    alarms::Bedtime bedtime(std::move(settingsProvider));
    ASSERT_TRUE(bedtime.decide(TimePointFromStringWithShift("2022-11-11 05:02:00")));
    ASSERT_TRUE(bedtime.decide(TimePointFromStringWithShift("2022-12-03 05:02:01")));
    ASSERT_TRUE(bedtime.decide(TimePointFromStringWithShift("2022-12-03 05:02:03")));
    ASSERT_TRUE(bedtime.decide(TimePointFromStringWithShift("2022-12-03 05:02:55")));
    ASSERT_FALSE(bedtime.decide(TimePointFromStringWithShift("2022-11-11 07:32:00")));
    ASSERT_FALSE(bedtime.decide(TimePointFromStringWithShift("2022-11-11 05:01:59")));
}

TEST(PreWakeUp, TimePointIsNotRoundedToFullMinute)
{
    alarms::PreWakeUp preWakeUp(std::make_unique<MockedPreWakeUpSettingsProvider>());
    auto event           = AlarmEventRecord(1, AlarmTime{5h, 5min}, defMusic, defEnabled, defSnooze, defRRule);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromStringWithShift("2022-11-11 05:00:00"));
    EXPECT_TRUE(static_cast<EventInfo>(nextEvent).isValid());

    const auto decision = preWakeUp.decide(timeInjector(), nextEvent);
    ASSERT_TRUE(decision.timeForChime);
    ASSERT_FALSE(decision.timeForFrontlight);
}

class BellAlarmOperationsFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        auto alarmRepoMock = std::make_unique<MockAlarmEventsRepository>();
        auto event         = AlarmEventRecord(1, AlarmTime{9h, 0min}, defMusic, defEnabled, defSnooze, defRRule);
        alarmRepoMock->nextRecords.push_back(event);
        chimeHandler      = std::make_shared<MockAlarmHandler>();
        frontlightHandler = std::make_shared<MockAlarmHandler>();

        alarmOperations = ::getMockedAlarmOperations(alarmRepoMock,
                                                     std::make_unique<MockedPreWakeUpSettingsProvider>(),
                                                     std::make_unique<MockedSnoozeChimeSettingsProvider>(),
                                                     std::make_unique<MockedBedtimeModel>());
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::PreWakeUpChime, chimeHandler);
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::PreWakeUpFrontlight, frontlightHandler);
        alarmOperations->updateEventsCache(TimePointFromStringWithShift("2022-11-11 08:00:00"));
    }

    std::unique_ptr<alarms::IAlarmOperations> alarmOperations;
    std::shared_ptr<MockAlarmHandler> chimeHandler;
    std::shared_ptr<MockAlarmHandler> frontlightHandler;
};

TEST_F(BellAlarmOperationsFixture, PreWakeUp_TooEarlyToHandle)
{
    EXPECT_CALL(*chimeHandler, handle(testing::_)).Times(0);
    EXPECT_CALL(*frontlightHandler, handle(testing::_)).Times(0);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 08:54:00"));
}

TEST_F(BellAlarmOperationsFixture, PreWakeUp_TimeToPlayChime)
{
    EXPECT_CALL(*chimeHandler, handle(testing::_)).Times(1);
    EXPECT_CALL(*frontlightHandler, handle(testing::_)).Times(0);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 08:55:00"));
}

TEST_F(BellAlarmOperationsFixture, PreWakeUp_TimeToLightFrontlight)
{
    EXPECT_CALL(*chimeHandler, handle(testing::_)).Times(0);
    EXPECT_CALL(*frontlightHandler, handle(testing::_)).Times(1);
    alarmOperations->minuteUpdated(TimePointFromStringWithShift("2022-11-11 08:58:00"));
}
