// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <AlarmOperations.hpp>
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

    std::unique_ptr<alarms::IAlarmOperations> getMockedAlarmOperations(
        std::unique_ptr<MockAlarmEventsRepository> &alarmRepo,
        std::unique_ptr<alarms::PreWakeUpSettingsProvider> &&preWakeUpSettingsProvider,
        std::unique_ptr<alarms::SnoozeChimeSettingsProvider> &&snoozeChimeSettingsProvider)
    {
        return std::make_unique<alarms::AlarmOperations>(std::move(alarmRepo),
                                                         timeInjector,
                                                         std::move(preWakeUpSettingsProvider),
                                                         std::move(snoozeChimeSettingsProvider));
    }
} // namespace

// This function replaces the `getMockedAlarmOperations` function from `service-time/tests/tests-AlarmOperations.cpp`
std::unique_ptr<alarms::IAlarmOperations> AlarmOperationsFixture::getMockedAlarmOperations(
    std::unique_ptr<MockAlarmEventsRepository> &alarmRepo)
{
    return ::getMockedAlarmOperations(alarmRepo,
                                      std::make_unique<MockedPreWakeUpSettingsProvider>(),
                                      std::make_unique<MockedSnoozeChimeSettingsProvider>());
}

TEST(PreWakeUp, TooEarlyForPreWakeUp)
{
    alarms::PreWakeUp preWakeUp(std::make_unique<MockedPreWakeUpSettingsProvider>());
    auto event           = AlarmEventRecord(1,
                                  defName,
                                  TimePointFromString("2022-11-11 05:06:00"),
                                  defDuration,
                                  defAllDay,
                                  defRRule,
                                  defMusic,
                                  defEnabled,
                                  defSnooze);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromString("2022-11-11 05:00:00"));
    EXPECT_TRUE(static_cast<EventInfo>(nextEvent).isValid());

    const auto decision = preWakeUp.decide(timeInjector(), nextEvent);
    ASSERT_FALSE(decision.timeForChime);
    ASSERT_FALSE(decision.timeForFrontlight);
}

TEST(PreWakeUp, TimeToPlayChime)
{
    alarms::PreWakeUp preWakeUp(std::make_unique<MockedPreWakeUpSettingsProvider>());
    auto event           = AlarmEventRecord(1,
                                  defName,
                                  TimePointFromString("2022-11-11 05:05:00"),
                                  defDuration,
                                  defAllDay,
                                  defRRule,
                                  defMusic,
                                  defEnabled,
                                  defSnooze);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromString("2022-11-11 05:00:00"));
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
    auto event           = AlarmEventRecord(1,
                                  defName,
                                  TimePointFromString("2022-11-11 05:05:00"),
                                  defDuration,
                                  defAllDay,
                                  defRRule,
                                  defMusic,
                                  defEnabled,
                                  defSnooze);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromString("2022-11-11 05:00:00"));
    EXPECT_TRUE(static_cast<EventInfo>(nextEvent).isValid());

    const auto decision = preWakeUp.decide(timeInjector(), nextEvent);
    ASSERT_FALSE(decision.timeForChime);
    ASSERT_FALSE(decision.timeForFrontlight);
}

TEST(PreWakeUp, TimeToLightFrontlight)
{
    alarms::PreWakeUp preWakeUp(std::make_unique<MockedPreWakeUpSettingsProvider>());
    auto event           = AlarmEventRecord(1,
                                  defName,
                                  TimePointFromString("2022-11-11 05:02:00"),
                                  defDuration,
                                  defAllDay,
                                  defRRule,
                                  defMusic,
                                  defEnabled,
                                  defSnooze);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromString("2022-11-11 05:00:00"));
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
    auto event           = AlarmEventRecord(1,
                                  defName,
                                  TimePointFromString("2022-11-11 05:02:00"),
                                  defDuration,
                                  defAllDay,
                                  defRRule,
                                  defMusic,
                                  defEnabled,
                                  defSnooze);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromString("2022-11-11 05:00:00"));
    EXPECT_TRUE(static_cast<EventInfo>(nextEvent).isValid());

    const auto decision = preWakeUp.decide(timeInjector(), nextEvent);
    ASSERT_FALSE(decision.timeForChime);
    ASSERT_FALSE(decision.timeForFrontlight);
}

TEST(PreWakeUp, TimePointIsNotRoundedToFullMinute)
{
    alarms::PreWakeUp preWakeUp(std::make_unique<MockedPreWakeUpSettingsProvider>());
    auto event           = AlarmEventRecord(1,
                                  defName,
                                  TimePointFromString("2022-11-11 05:05:01"),
                                  defDuration,
                                  defAllDay,
                                  defRRule,
                                  defMusic,
                                  defEnabled,
                                  defSnooze);
    const auto nextEvent = event.getNextSingleEvent(TimePointFromString("2022-11-11 05:00:00"));
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
        alarmRepoMock->nextRecords.push_back(AlarmEventRecord(1,
                                                              defName,
                                                              TimePointFromString("2022-11-11 09:00:00"),
                                                              defDuration,
                                                              defAllDay,
                                                              defRRule,
                                                              defMusic,
                                                              defEnabled,
                                                              defSnooze));
        chimeHandler      = std::make_shared<MockAlarmHandler>();
        frontlightHandler = std::make_shared<MockAlarmHandler>();

        alarmOperations = ::getMockedAlarmOperations(alarmRepoMock,
                                                     std::make_unique<MockedPreWakeUpSettingsProvider>(),
                                                     std::make_unique<MockedSnoozeChimeSettingsProvider>());
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::PreWakeUpChime, chimeHandler);
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::PreWakeUpFrontlight, frontlightHandler);
        alarmOperations->updateEventsCache(TimePointFromString("2022-11-11 08:00:00"));
    }

    std::unique_ptr<alarms::IAlarmOperations> alarmOperations;
    std::shared_ptr<MockAlarmHandler> chimeHandler;
    std::shared_ptr<MockAlarmHandler> frontlightHandler;
};

TEST_F(BellAlarmOperationsFixture, PreWakeUp_TooEarlyToHandle)
{
    EXPECT_CALL(*chimeHandler, handle(testing::_)).Times(0);
    EXPECT_CALL(*frontlightHandler, handle(testing::_)).Times(0);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 08:54:00"));
}

TEST_F(BellAlarmOperationsFixture, PreWakeUp_TimeToPlayChime)
{
    EXPECT_CALL(*chimeHandler, handle(testing::_)).Times(1);
    EXPECT_CALL(*frontlightHandler, handle(testing::_)).Times(0);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 08:55:00"));
}

TEST_F(BellAlarmOperationsFixture, PreWakeUp_TimeToLightFrontlight)
{
    EXPECT_CALL(*chimeHandler, handle(testing::_)).Times(0);
    EXPECT_CALL(*frontlightHandler, handle(testing::_)).Times(1);
    alarmOperations->minuteUpdated(TimePointFromString("2022-11-11 08:58:00"));
}
