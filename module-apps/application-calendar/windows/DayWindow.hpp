#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <ListView.hpp>
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Label.hpp"
#include "../models/CalendarModel.hpp"

namespace app
{
    namespace calendarStyle
    {
        const int cross_x    = 48;
        const int cross_y    = 55;
        const int arrow_x    = 30;
        const int arrow_y    = 62;
        const int listView_x = 16;
        const int listView_y = 110;
        const int listView_w = 448;
        const int listView_h = 440;
    } // namespace calendarStyle

    class DayWindow : public gui::AppWindow
    {
        gui::Image *leftArrowImage   = nullptr;
        gui::Image *newDayEventImage = nullptr;
        gui::ListView *list          = nullptr;
        CalendarModel *model         = nullptr;

      public:
        DayWindow(Application *app, std::string name);
        virtual ~DayWindow() = default;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace app */
