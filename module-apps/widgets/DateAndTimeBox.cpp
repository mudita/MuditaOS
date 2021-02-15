// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateAndTimeBox.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"

namespace gui
{
    DateAndTimeBox::DateAndTimeBox(app::Application *application, Item *parent) : VBox(parent), app(application)
    {
        setMinimumSize(style::window::default_body_width, 400);
        setEdges(RectangleEdge::None);

        dateWidget = new DateWidget(this);

        startTimeWidget = new TimeWidget(
            this,
            utils::localize.get("app_calendar_new_edit_event_start"),
            TimeWidget::Type::Start,
            [&](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
            [&]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

        endTimeWidget = new TimeWidget(
            this,
            utils::localize.get("app_calendar_new_edit_event_end"),
            TimeWidget::Type::End,
            [&](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
            [&]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

        endTimeWidget->setConnectionToSecondItem(startTimeWidget);
        startTimeWidget->setConnectionToSecondItem(endTimeWidget);

        startTimeWidget->setConnectionToDateItem(dateWidget);
        endTimeWidget->setConnectionToDateItem(dateWidget);

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            return getFocusItem()->onInput(event);
        };

        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            dateWidget->onLoadCallback(event);
            startTimeWidget->onLoadCallback(event);
            endTimeWidget->onLoadCallback(event);
        };

        onSaveCallback = [&](std::shared_ptr<EventsRecord> event) -> bool {
            startTimeWidget->onSaveCallback(event);
            endTimeWidget->onSaveCallback(event);
            return true;
        };
    }
} // namespace gui
