#pragma once

#include "application-calendar/models/DayEventsModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include "NoEvents.hpp"

#include <ListView.hpp>
#include <gui/widgets/Item.hpp>
#include <gui/widgets/Label.hpp>

namespace gui
{
    class DayEventsWindow : public gui::AppWindow
    {
        gui::Image *leftArrowImage                               = nullptr;
        gui::Image *newDayEventImage                             = nullptr;
        gui::ListView *dayEventsList                             = nullptr;
        std::shared_ptr<DayEventsModel> dayEventsModel           = nullptr;

      public:
        DayEventsWindow(app::Application *app, std::string name);

        bool onInput(const gui::InputEvent &inputEvent) override;
        bool onDatabaseMessage(sys::Message *msgl) override;
        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace app */
