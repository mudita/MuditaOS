#pragma once
#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>
#include <module-apps/Application.hpp>
#include <module-apps/application-calendar/ApplicationCalendar.hpp>

namespace gui

{
    class RepeatAndReminderItem : public CalendarListItem
    {
        gui::HBox *hBox           = nullptr;
        gui::VBox *repeatVBox     = nullptr;
        gui::VBox *reminderVBox   = nullptr;
        gui::Label *repeatTitle   = nullptr;
        gui::Label *repeat        = nullptr;
        gui::Label *reminderTitle = nullptr;
        gui::Label *reminder      = nullptr;
        app::ApplicationCalendar *app = nullptr;

      public:
        RepeatAndReminderItem(app::ApplicationCalendar *application);
        virtual ~RepeatAndReminderItem() = default;

        void descriptionHandler();
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
