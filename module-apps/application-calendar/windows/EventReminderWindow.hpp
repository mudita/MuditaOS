#pragma once
#include "application-calendar/models/EventDetailModel.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <Style.hpp>
#include <ListView.hpp>
#include <module-db/Interface/EventsRecord.hpp>

#include <Text.hpp>
#include <Label.hpp>
#include <module-sys/Service/Timer.hpp>
//#include <module-apps/GuiTimer.hpp>
//#include <Label.hpp>
//#include <functional>

namespace gui
{
    class EventReminderWindow : public gui::AppWindow
    {
      private:
        // uint32_t timerId = 0;
        // app::GuiTimer reminderTimer;
        std::unique_ptr<sys::Timer> reminderTimer;

      protected:
        std::string prevWindowName                = "";
        std::shared_ptr<EventsRecord> eventRecord = nullptr;

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
        virtual ~EventReminderWindow() override;

        virtual void onClose() override;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        auto handleSwitchData(SwitchData *data) -> bool override;
    };

} /* namespace gui */
