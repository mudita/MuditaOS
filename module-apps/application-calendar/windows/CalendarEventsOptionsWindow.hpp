#pragma once
#include "OptionWindow.hpp"

namespace gui
{
    class CalendarEventsOptions : public OptionWindow
    {
      public:
        CalendarEventsOptions(app::Application *app);
        ~CalendarEventsOptions() override = default;

      private:
        auto eventsOptionsList() -> std::list<gui::Option>;
        auto eventDelete() -> bool;
    };
}; // namespace gui
