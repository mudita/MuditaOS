// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventDetailDescriptionItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <Style.hpp>
#include <time/time_conversion.hpp>
#include <module-utils/date/include/date/date.h>
#include <module-utils/time/TimeRangeParser.hpp>

namespace gui
{

    EventDetailDescriptionItem::EventDetailDescriptionItem()
    {
        setEdges(RectangleEdge::None);
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::eventDetail::height_min);
        setMaximumSize(style::window::default_body_width, style::window::calendar::item::eventDetail::height_max);
        setMargins(gui::Margins(style::margins::small, style::window::calendar::item::eventDetail::margin_top, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);

        hBox = new HBox(vBox, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width, style::window::calendar::item::eventDetail::title_h);
        hBox->setEdges(RectangleEdge::None);

        squareImage = new gui::Image();
        squareImage->set("Rectangle");
        squareImage->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        hBox->addWidget(squareImage);

        title = new gui::Label(hBox, 0, 0, 0, 0);
        title->setMinimumSize(style::window::default_body_width - 40,
                              style::window::calendar::item::eventDetail::title_h);
        title->setMargins(gui::Margins(style::margins::small, 0, 0, 0));
        title->setEdges(RectangleEdge::None);
        title->setFont(style::window::font::small);
        title->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        title->activeItem = false;

        eventTime = new gui::Label(vBox, 0, 0, 0, 0);
        eventTime->setMinimumSize(style::window::default_body_width,
                                  style::window::calendar::item::eventDetail::label_h);
        eventTime->setEdges(RectangleEdge::None);
        eventTime->setMargins(
            gui::Margins(0, style::window::calendar::item::eventDetail::event_time_margin, 0, style::margins::small));
        eventTime->setFont(style::window::font::bigbold);
        eventTime->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        eventTime->activeItem = false;

        description = new gui::Text(vBox, 0, 0, 0, 0);
        description->setMinimumSize(style::window::default_body_width,
                                    style::window::calendar::item::eventDetail::label_h);
        description->setEdges(RectangleEdge::None);
        description->setMaximumSize(style::window::default_body_width,
                                    2 * style::window::calendar::item::eventDetail::label_h);
        description->setFont(style::window::font::big);
        description->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        description->setEditMode(EditMode::Browse);

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? vBox : nullptr);
            return true;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        descriptionHandler();
    }

    void EventDetailDescriptionItem::descriptionHandler()
    {
        title->setText(utils::translate("app_calendar_event_detail"));
        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            description->setText(event->title);
            eventTime->setText(
                utils::time::TimeRangeParser().getCalendarTimeString(event->date_from, event->date_till));
        };
    }

} /* namespace gui */
