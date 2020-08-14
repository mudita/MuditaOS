#pragma once
#include "application-calendar/models/AllEventsModel.hpp"
#include "application-calendar/widgets/CalendarListView.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <ListView.hpp>

namespace gui
{
    class AllEventsWindow : public gui::AppWindow
    {
        gui::Image *leftArrowImage   = nullptr;
        gui::Image *newDayEventImage = nullptr;

        gui::ListView *allEventsList                   = nullptr;
        std::shared_ptr<AllEventsModel> allEventsModel = nullptr;

      public:
        AllEventsWindow(app::Application *app, std::string name);
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onDatabaseMessage(sys::Message *msgl) override;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;

        bool checkEmpty = false;
    };

} // namespace gui
