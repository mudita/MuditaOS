// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "application-calendar/models/EventDetailModel.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <Style.hpp>
#include <module-db/Interface/EventsRecord.hpp>
#include <Text.hpp>
#include <Label.hpp>
#include <module-sys/Timers/TimerHandle.hpp>

namespace gui
{
    class Image;

    class EventReminderWindow : public gui::AppWindow
    {
      private:
        sys::TimerHandle reminderTimer;

      protected:
        std::shared_ptr<EventsRecord> eventRecord;

        gui::VBox *body         = nullptr;
        Label *dateLabel        = nullptr;
        Label *timeLabel        = nullptr;
        Label *descriptionLabel = nullptr;
        Image *topImage         = nullptr;
        Image *bottomImage      = nullptr;

        void startTimer();
        void destroyTimer();
        void reminderTimerCallback();
        void closeReminder();

      public:
        EventReminderWindow(app::Application *app, std::string name);
        virtual ~EventReminderWindow();

        void onClose() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
        auto handleSwitchData(SwitchData *data) -> bool override;
    };

} /* namespace gui */
