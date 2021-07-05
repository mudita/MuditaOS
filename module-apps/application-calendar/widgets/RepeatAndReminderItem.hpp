// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>
#include <apps-common/Application.hpp>
#include <module-apps/application-calendar/ApplicationCalendar.hpp>

namespace gui

{
    class RepeatAndReminderItem : public CalendarListItem
    {
        gui::HBox *hBox           = nullptr;
        gui::VBox *repeatVBox     = nullptr;
        gui::VBox *reminderVBox   = nullptr;
        gui::HBox *repeatHBox         = nullptr;
        gui::HBox *reminderHBox       = nullptr;
        gui::Label *repeatTitle   = nullptr;
        gui::Label *repeat        = nullptr;
        gui::Label *reminderTitle = nullptr;
        gui::Label *reminder      = nullptr;
        app::ApplicationCalendar *app = nullptr;

      public:
        RepeatAndReminderItem(app::ApplicationCalendar *application);
        virtual ~RepeatAndReminderItem() = default;

        void descriptionHandler();
    };

} /* namespace gui */
