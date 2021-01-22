// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateTimeItem.hpp"
#include "CalendarStyle.hpp"
#include <InputEvent.hpp>

namespace gui
{

    DateTimeItem::DateTimeItem(app::Application *app, DateTimeType dateTimeType) : app(app), dateTimeType(dateTimeType)
    {
        buildInterface();
        applyCallbacks();
    }

    void DateTimeItem::buildInterface()
    {
        using namespace style::window::calendar::item;
        setMinimumSize(style::window::calendar::item::width, severalOptions::height);
        setEdges(RectangleEdge::Bottom);
        setPenWidth(style::window::default_border_rect_no_focus);
        setMargins(gui::Margins(style::window::calendar::leftMargin, style::margins::big, 0, style::margins::big));

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdge::None);

        description = new gui::Label(vBox, 0, 0, 0, 0);
        description->setMinimumSize(style::window::calendar::item::width, severalOptions::label_h);
        description->setEdges(gui::RectangleEdge::None);
        description->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        description->setFont(style::window::font::small);
        if (dateTimeType == DateTimeType::Start) {
            description->setText(utils::localize.get("app_calendar_start_date"));
        }
        else {
            description->setText(utils::localize.get("app_calendar_end_date"));
        }

        hBox = new gui::HBox(vBox, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::calendar::item::width, dateTime::image);
        hBox->setEdges(gui::RectangleEdge::None);

        dateTime = new gui::Label(hBox, 0, 0, 0, 0);
        dateTime->setMinimumSize(dateTime::text_w, dateTime::image);
        dateTime->setEdges(gui::RectangleEdge::None);
        dateTime->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        dateTime->setFont(style::window::font::medium);

        image = new gui::Image(hBox, 0, 0, 0, 0, "calendar_icon");
        image->setMinimumSize(dateTime::image, dateTime::image);
    }

    void DateTimeItem::applyCallbacks()
    {
        focusChangedCallback = [&](Item &item) {
            if (item.focus) {
                app->getCurrentWindow()->bottomBarTemporaryMode(utils::localize.get("app_calendar_edit"), false);
            }
            else {
                app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };

        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            if (dateTimeType == DateTimeType::Start) {
                dateTime->setText(getSpecificDateTimeFormat(event->date_from));
            }
            else {
                dateTime->setText(getSpecificDateTimeFormat(event->date_till));
            }
        };
    }

    bool DateTimeItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }

    std::string DateTimeItem::getSpecificDateTimeFormat(TimePoint time)
    {
        return TimePointToLocalizedDateString(time, "%A, %e %b %y") + ", " +
               TimePointToLocalizedTimeString(time, "%I:%M %p");
    }
} // namespace gui
