#pragma once

#include "application-calendar/models/CalendarAllEventsModel.hpp"
#include "application-calendar/widgets/CalendarListView.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <ListView.hpp>

namespace app
{
    class AllEventsWindow : public gui::AppWindow
    {
        gui::Image *leftArrowImage   = nullptr;
        gui::Image *newDayEventImage = nullptr;

        gui::ListView *allEventsList                                   = nullptr;
        std::shared_ptr<CalendarAllEventsModel> calendarAllEventsModel = nullptr;

      public:
        AllEventsWindow(Application *app, std::string name);
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace app */
