// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-alarm-clock/presenter/AlarmReminderWindowPresenter.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <Style.hpp>
#include <Text.hpp>
#include <Label.hpp>
#include <module-db/Interface/AlarmsRecord.hpp>
#include <module-sys/Service/Timer.hpp>

namespace app::alarmClock
{
    class AlarmReminderWindow : public gui::AppWindow, public AlarmReminderWindowContract::View
    {
      private:
        std::unique_ptr<sys::Timer> reminderTimer;
        std::unique_ptr<sys::Timer> musicTimer;
        std::unique_ptr<sys::Timer> delayTimer;
        std::unique_ptr<AlarmReminderWindowContract::Presenter> presenter;
        std::shared_ptr<AlarmsRecord> alarmRecord;
        std::shared_ptr<AlarmsRecord> previousAlarmRecord;

        gui::VBox *body         = nullptr;
        gui::HBox *hBox         = nullptr;
        gui::VBox *vBox         = nullptr;
        gui::VBox *snoozeVBox   = nullptr;
        gui::Label *alarmLabel  = nullptr;
        gui::Label *timeLabel   = nullptr;
        gui::Label *snoozeLabel = nullptr;
        gui::Image *leftImage   = nullptr;
        gui::Image *rightImage  = nullptr;
        gui::Image *snoozeImage = nullptr;

        uint32_t elapsedMinutes = 0;
        uint32_t previousElapsedMinutes = 0;

        void startTimers();
        void startMusicTimer();
        void destroyTimers();
        void closeReminder();
        void closeReminderCallback();
        void loopMusic();
        void countElapsedMinutes();

      public:
        AlarmReminderWindow(app::Application *app,
                            std::unique_ptr<AlarmReminderWindowContract::Presenter> &&windowPresenter);
        ~AlarmReminderWindow() override;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        auto handleSwitchData(gui::SwitchData *data) -> bool override;
    };

} // namespace app::alarmClock
