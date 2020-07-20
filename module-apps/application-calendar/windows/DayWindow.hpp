#pragma once

#include "application-calendar/models/CalendarEventsModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include "NoEvents.hpp"

#include <ListView.hpp>
#include <gui/widgets/Item.hpp>
#include <gui/widgets/Label.hpp>

namespace app
{
    class DayWindow : public gui::AppWindow
    {
        gui::Image *leftArrowImage                               = nullptr;
        gui::Image *newDayEventImage                             = nullptr;
        gui::ListView *dayEventsList                             = nullptr;
        std::shared_ptr<CalendarEventsModel> calendarEventsModel = nullptr;

      public:
        DayWindow(Application *app, std::string name);

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace app */
