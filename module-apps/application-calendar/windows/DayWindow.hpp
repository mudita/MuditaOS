#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <ListView.hpp>
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Label.hpp"
#include "../models/CalendarModel.hpp"
#include <Text.hpp>

namespace app
{

    /*
     *
     */
    class DayWindow : public gui::AppWindow
    {
        gui::Image *leftArrowImage   = nullptr;
        gui::Image *newDayEventImage = nullptr;
        gui::Image *emptyListImage   = nullptr;
        gui::ListView *list          = nullptr;
        gui::Text *emptyListText     = nullptr;
        CalendarModel *model         = nullptr;
        void emptyDay();

      public:
        std::string name;
        DayWindow(Application *app, std::string name);
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace app */
