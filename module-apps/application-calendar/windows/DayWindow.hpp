#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <ListView.hpp>
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Label.hpp"
#include "application-calendar/models/CalendarModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"

namespace app
{
    class DayWindow : public gui::AppWindow
    {
        gui::Image *leftArrowImage                   = nullptr;
        gui::Image *newDayEventImage                 = nullptr;
        gui::ListView *list                          = nullptr;
        std::shared_ptr<CalendarModel> calendarModel = nullptr;

      public:
        DayWindow(Application *app, std::string name);
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace app */
