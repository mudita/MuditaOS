// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAlarmModel.hpp>
#include <common/models/UserSessionModel.hpp>
#include <common/models/BatteryLevelNotificationModel.hpp>
#include <common/models/QuoteModel.hpp>
#include <common/layouts/BaseHomeScreenLayoutProvider.hpp>
#include <common/layouts/HomeScreenLayouts.hpp>
#include <gui/Common.hpp>
#include <gui/input/InputEvent.hpp>
#include <module-utils/EventStore/EventStore.hpp>
#include <Timers/TimerHandle.hpp>
#include <Temperature.hpp>
#include <time/time_locale.hpp>
#include <utf8/UTF8.hpp>

#include <chrono>

namespace app
{
    class AbstractTimeModel;
    class AbstractBatteryModel;
    class AbstractBatteryLevelNotificationModel;
    class ApplicationCommon;
    class TemperatureModel;
    class AbstractUsbStatusModel;
    class AbstractUserSessionModel;
    class ProgressTimerWithSnoozeTimer;
} // namespace app

namespace gui
{
    class AppWindow;
    class BaseHomeScreenLayoutProvider;
} // namespace gui

namespace db
{
    class NotificationMessage;
}

namespace app::home_screen
{
    class AbstractController;
    class AbstractTemperatureModel;

    class AbstractView
    {
      public:
        virtual ~AbstractView() noexcept = default;

        /// Alarm widget related API
        virtual void setViewState(ViewState state)                           = 0;
        [[nodiscard]] virtual std::time_t getAlarmTime() const               = 0;
        virtual void setAlarmTime(std::time_t time)                          = 0;
        virtual void setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;
        virtual void setSnoozeFormat(utils::time::Locale::TimeFormat fmt)    = 0;

        /// Time widget related API
        virtual void setTime(std::time_t time)                          = 0;
        virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;

        /// Bottom box related API(descriptionn battery or time)
        virtual void setTemperature(utils::temperature::Temperature newTemp)    = 0;
        virtual void setTextDescription(const UTF8 &desc)                       = 0;
        virtual void setBatteryLevelState(const Store::Battery &batteryContext) = 0;
        virtual void updateUsbStatus(bool isConnected)                          = 0;
        virtual void setQuoteText(const UTF8 &quoteContent, const UTF8 &quoteAuthor) = 0;

        /// Various
        virtual void setLayout(gui::LayoutGenerator layoutGenerator) = 0;
        virtual void setSnoozeTime(std::time_t time)                 = 0;
    };

    class AbstractPresenter : public BasePresenter<AbstractView>
    {
      public:
        virtual ~AbstractPresenter() noexcept                                                    = default;
        virtual void createData()                                                                = 0;
        virtual gui::RefreshModes handleUpdateTimeEvent()                                        = 0;
        virtual bool handleInputEvent(const gui::InputEvent &inputEvent)                         = 0;
        virtual void onBeforeShow()                                                              = 0;
        virtual void onDatabaseMessage(db::NotificationMessage *msg)                             = 0;
        virtual void refreshWindow()                                                             = 0;
        virtual void spawnTimer(std::chrono::milliseconds timeout = defaultTimeout)              = 0;
        virtual void detachTimer()                                                               = 0;
        virtual void handleAlarmRingingEvent()                                                   = 0;
        virtual void handleAlarmModelReady()                                                     = 0;
        virtual void handleBatteryStatus()                                                       = 0;
        virtual void setSnoozeTimer(std::unique_ptr<app::ProgressTimerWithSnoozeTimer> &&_timer) = 0;
        virtual void startSnoozeTimer(std::chrono::seconds snoozeDuration)                       = 0;
        virtual void stopSnoozeTimer()                                                           = 0;
        virtual void restartSnoozeTimer(std::chrono::seconds snoozeDuration)                     = 0;
        [[nodiscard]] virtual std::uint32_t getBatteryLvl() const                                = 0;
        [[nodiscard]] virtual bool isBatteryCharging() const                                     = 0;
        [[nodiscard]] virtual bool isAlarmActivatedByLatch() const                               = 0;
        virtual void setLayout(gui::LayoutGenerator layoutGenerator)                             = 0;
        virtual void incAlarmMinute()                                                            = 0;
        virtual void decAlarmMinute()                                                            = 0;
        virtual void switchToMenu()                                                              = 0;
        virtual void switchToBatteryStatus()                                                     = 0;
        virtual void switchToLowBatteryWarning()                                                 = 0;
        virtual UTF8 getGreeting()                                                               = 0;
        virtual void updateUsbStatus()                                                           = 0;
        virtual void handleLowBatteryWarning()                                                   = 0;
        virtual bool isLowBatteryWarningNeeded()                                                 = 0;
        virtual void updateBatteryLevelInterval()                                                = 0;
        virtual void refreshUserSession()                                                        = 0;
        [[nodiscard]] virtual bool isLowBatteryLevel() const                                     = 0;
        virtual void requestQuote() const                                                        = 0;

        static constexpr auto defaultTimeout = std::chrono::milliseconds{5000};
    };

    class HomeScreenPresenter : public AbstractPresenter
    {
      public:
        HomeScreenPresenter(ApplicationCommon *app,
                            AbstractAlarmModel &alarmModel,
                            AbstractBatteryModel &batteryModel,
                            AbstractTemperatureModel &temperatureModel,
                            AbstractTimeModel &timeModel,
                            AbstractUserSessionModel &userSessionModel,
                            AbstractBatteryLevelNotificationModel &batteryLevelNotificationModel,
                            AbstractUsbStatusModel &usbStatusModel,
                            AbstractQuoteModel &quoteModel);
        virtual ~HomeScreenPresenter();
        HomeScreenPresenter()        = delete;
        HomeScreenPresenter &operator=(const HomeScreenPresenter &oth) = delete;
        HomeScreenPresenter &operator=(HomeScreenPresenter &&oth) = delete;
        HomeScreenPresenter(const HomeScreenPresenter &oth)       = delete;
        HomeScreenPresenter(HomeScreenPresenter &&oth)            = delete;

        void createData() override;
        gui::RefreshModes handleUpdateTimeEvent() override;
        bool handleInputEvent(const gui::InputEvent &inputEvent) override;
        void onBeforeShow() override;
        void onDatabaseMessage(db::NotificationMessage *msg) override;
        void refreshWindow() override;

        void spawnTimer(std::chrono::milliseconds timeout = defaultTimeout) override;
        void detachTimer() override;
        void handleAlarmRingingEvent() override;
        void handleAlarmModelReady() override;
        void handleBatteryStatus() override;

        void setSnoozeTimer(std::unique_ptr<app::ProgressTimerWithSnoozeTimer> &&_timer) override;
        void startSnoozeTimer(std::chrono::seconds snoozeDuration) override;
        void stopSnoozeTimer() override;
        void restartSnoozeTimer(std::chrono::seconds snoozeDuration) override;
        [[nodiscard]] std::uint32_t getBatteryLvl() const override;
        [[nodiscard]] bool isBatteryCharging() const override;
        [[nodiscard]] bool isAlarmActivatedByLatch() const override;
        void updateUsbStatus() override;

        void incAlarmMinute() override;
        void decAlarmMinute() override;
        void switchToMenu() override;
        void switchToBatteryStatus() override;
        void switchToLowBatteryWarning() override;
        UTF8 getGreeting() override;
        void handleLowBatteryWarning() override;
        bool isLowBatteryWarningNeeded() override;
        void updateBatteryLevelInterval() override;
        void refreshUserSession() override;
        [[nodiscard]] bool isLowBatteryLevel() const override;
        void requestQuote() const override;

        void setLayout(gui::LayoutGenerator layoutGenerator) override;

      private:
        ApplicationCommon *app;
        sys::TimerHandle timer;
        AbstractAlarmModel &alarmModel;
        AbstractBatteryModel &batteryModel;
        AbstractTemperatureModel &temperatureModel;
        AbstractTimeModel &timeModel;
        AbstractUserSessionModel &userSessionModel;
        AbstractBatteryLevelNotificationModel &batteryLevelNotificationModel;
        AbstractUsbStatusModel &usbStatusModel;
        AbstractQuoteModel &quoteModel;
        std::unique_ptr<AbstractController> stateController;
        std::unique_ptr<ProgressTimerWithSnoozeTimer> snoozeTimer;
        std::unique_ptr<std::mt19937> rngEngine;

        gui::RefreshModes handleCyclicDeepRefresh();

        static constexpr auto timerName  = "HS_timer";
        static constexpr auto snoozeTick = std::chrono::seconds(1);
    };
} // namespace app::home_screen
