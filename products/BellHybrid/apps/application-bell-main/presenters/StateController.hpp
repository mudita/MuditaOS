// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractAlarmModel.hpp>
#include <gui/input/InputEvent.hpp>

#include <chrono>
#include <memory>

namespace app
{
    class AbstractTimeModel;
    class AbstractBatteryModel;
}

namespace app::home_screen
{

    inline constexpr auto defaultAlarmRingingTime = std::chrono::minutes(5);
    inline constexpr auto defaultAlarmSetTime     = std::chrono::seconds{10};

    class AbstractView;
    class AbstractPresenter;
    class AbstractTemperatureModel;
    class AbstractController
    {
      public:
        virtual ~AbstractController() noexcept = default;

        virtual void resetStateMachine()                                 = 0;
        virtual bool handleInputEvent(const gui::InputEvent &inputEvent) = 0;
        virtual bool handleTimerEvent()                                  = 0;
        virtual bool handleTimeUpdateEvent()                             = 0;
        virtual bool handleAlarmRingingEvent()                           = 0;
        virtual bool handleAlarmModelReady()                             = 0;
        virtual bool handleBatteryStatus()                               = 0;
        virtual bool isSnoozeAllowed()                                   = 0;
        virtual void snooze(bool ctrl)                                   = 0;
    };

    class StateController : public AbstractController
    {
      public:
        StateController(AbstractView &view,
                        AbstractPresenter &presenter,
                        AbstractBatteryModel &batteryModel,
                        AbstractTemperatureModel &temperatureModel,
                        AbstractAlarmModel &alarmModel,
                        AbstractTimeModel &timeModel);
        ~StateController();

        void resetStateMachine() override;
        bool handleInputEvent(const gui::InputEvent &inputEvent) override;
        bool handleTimerEvent() override;
        bool handleTimeUpdateEvent() override;
        bool handleAlarmRingingEvent() override;
        bool handleAlarmModelReady() override;
        bool handleBatteryStatus() override;
        bool isSnoozeAllowed() override;
        void snooze(bool ctrl) override;

      private:
        class Impl;
        std::unique_ptr<Impl> pimpl;
        AbstractPresenter &presenter;
    };
} // namespace app::home_screen
