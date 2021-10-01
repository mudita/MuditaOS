// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "models/TemperatureModel.hpp"

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAlarmModel.hpp>
#include <gui/input/InputEvent.hpp>
#include <module-utils/EventStore/EventStore.hpp>
#include <Timers/TimerHandle.hpp>
#include <Temperature.hpp>
#include <time/time_locale.hpp>
#include <utf8/UTF8.hpp>

#include <chrono>
#include <functional>

namespace app
{
    class AbstractTimeModel;
    class ApplicationCommon;
} // namespace app

namespace gui
{
    class AppWindow;
}

namespace db
{
    class NotificationMessage;
}

namespace app::home_screen
{
    class AbstractController;
    class AbstractBatteryModel;
    class AbstractTemperatureModel;

    class AbstractView
    {
      public:
        virtual ~AbstractView() noexcept = default;

        /// Alarm widget related API
        virtual void setAlarmTriggered()                                     = 0;
        virtual void setAlarmSnoozed()                                       = 0;
        virtual void setAlarmActive(bool)                                    = 0;
        virtual void setAlarmEdit(bool)                                      = 0;
        virtual void setAlarmVisible(bool)                                   = 0;
        virtual void setAlarmTimeVisible(bool)                               = 0;
        virtual std::time_t getAlarmTime() const                             = 0;
        virtual void setAlarmTime(std::time_t time)                          = 0;
        virtual void setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;
        virtual void incAlarmMinute()                                        = 0;
        virtual void decAlarmMinute()                                        = 0;

        /// Time widget related API
        virtual void setTime(std::time_t time)                          = 0;
        virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;

        /// Bottom box related API(descriptionn battery or time)
        virtual void setTemperature(utils::temperature::Temperature newTemp)    = 0;
        virtual void setBottomDescription(const UTF8 &desc)                     = 0;
        virtual void setBatteryLevelState(const Store::Battery &batteryContext) = 0;

        /// Various
        virtual void switchToMenu() = 0;
    };

    class AbstractPresenter : public BasePresenter<AbstractView>
    {
      public:
        virtual ~AbstractPresenter() noexcept                                       = default;
        virtual void createData()                                                   = 0;
        virtual void handleUpdateTimeEvent()                                        = 0;
        virtual bool handleInputEvent(const gui::InputEvent &inputEvent)            = 0;
        virtual void onBeforeShow()                                                 = 0;
        virtual void onDatabaseMessage(db::NotificationMessage *msg)                = 0;
        virtual void refreshWindow()                                                = 0;
        virtual void spawnTimer(std::chrono::milliseconds timeout = defaultTimeout) = 0;
        virtual void detachTimer()                                                  = 0;
        virtual void handleAlarmRingingEvent()                                      = 0;
        virtual void handleAlarmModelReady()                                        = 0;

        static constexpr auto defaultTimeout = std::chrono::milliseconds{5000};
    };

    class HomeScreenPresenter : public AbstractPresenter
    {
      public:
        HomeScreenPresenter(ApplicationCommon *app,
                            std::unique_ptr<AbstractAlarmModel> alarmModel,
                            std::unique_ptr<AbstractBatteryModel> batteryModel,
                            std::unique_ptr<AbstractTemperatureModel> temperatureModel,
                            std::unique_ptr<AbstractTimeModel> timeModel);
        virtual ~HomeScreenPresenter();
        HomeScreenPresenter()        = delete;
        HomeScreenPresenter &operator=(const HomeScreenPresenter &oth) = delete;
        HomeScreenPresenter &operator=(HomeScreenPresenter &&oth) = delete;
        HomeScreenPresenter(const HomeScreenPresenter &oth)       = delete;
        HomeScreenPresenter(HomeScreenPresenter &&oth)            = delete;

        void createData() override;
        void handleUpdateTimeEvent() override;
        bool handleInputEvent(const gui::InputEvent &inputEvent) override;
        void onBeforeShow() override;
        void onDatabaseMessage(db::NotificationMessage *msg) override;
        void refreshWindow() override;

        void spawnTimer(std::chrono::milliseconds timeout = defaultTimeout) override;
        void detachTimer() override;
        void handleAlarmRingingEvent() override;
        void handleAlarmModelReady() override;

      private:
        ApplicationCommon *app;
        sys::TimerHandle timer;
        std::unique_ptr<AbstractAlarmModel> alarmModel;
        std::unique_ptr<AbstractBatteryModel> batteryModel;
        std::unique_ptr<AbstractTemperatureModel> temperatureModel;
        std::unique_ptr<AbstractTimeModel> timeModel;
        std::shared_ptr<AbstractController> stateController;

        static constexpr auto timerName = "HS_timer";
    };
} // namespace app::home_screen
