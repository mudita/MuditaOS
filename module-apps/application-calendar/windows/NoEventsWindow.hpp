#pragma once

#include "application-calendar/widgets/CalendarStyle.hpp"

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Label.hpp"
#include "application-messages/widgets/Icon.hpp"

namespace app
{
    class NoEventsWindow : public gui::AppWindow
    {
        gui::Image *leftArrowImage   = nullptr;
        gui::Image *newDayEventImage = nullptr;
        gui::Icon *emptyListIcon     = nullptr;

      public:
        NoEventsWindow(Application *app, std::string name);

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace app */
