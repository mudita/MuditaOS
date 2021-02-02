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
        std::unique_ptr<AlarmReminderWindowContract::Presenter> presenter;

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

        void closeReminder();
        void displayAlarm();
        AlarmsReminderModel::OnTimerCallback getCallback();

      public:
        AlarmReminderWindow(app::Application *app,
                            std::unique_ptr<AlarmReminderWindowContract::Presenter> &&windowPresenter);
        ~AlarmReminderWindow() override;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        gui::top_bar::Configuration configureTopBar(gui::top_bar::Configuration appConfiguration) override;
        auto handleSwitchData(gui::SwitchData *data) -> bool override;

        void handleAudioTokenUpdate(audio::Token token);
    };

} // namespace app::alarmClock
