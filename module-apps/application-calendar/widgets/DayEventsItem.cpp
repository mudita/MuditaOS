// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DayEventsItem.hpp"
#include "CalendarStyle.hpp"
#include <ListView.hpp>
#include <gui/widgets/Label.hpp>
#include <Style.hpp>
#include <module-utils/time/TimeRangeParser.hpp>

namespace gui
{

    DayEventsItem::DayEventsItem()
    {
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::dayEvents::height);
        setMargins(gui::Margins(0, style::margins::small, 0, 0));

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdge::None);
        hBox = new gui::HBox(vBox, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width, style::window::calendar::item::dayEvents::box_height);
        hBox->setMargins(gui::Margins(0, style::margins::small + style::margins::big, 0, 0));
        hBox->setEdges(gui::RectangleEdge::None);

        title = new gui::Label(hBox, 0, 0, 0, 0);
        title->setMinimumSize(style::window::calendar::item::dayEvents::title_w,
                              style::window::calendar::item::dayEvents::box_height);
        title->setMargins(gui::Margins(style::margins::small, 0, style::margins::small + style::margins::big, 0));
        title->setEdges(gui::RectangleEdge::None);
        title->setFont(style::window::font::bigbold);
        title->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        clock = new gui::Image(hBox, 0, 0, 0, 0, "small_tick_W_M");
        clock->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        description = new gui::Label(vBox, 0, 0, 0, 0);
        description->setMinimumSize(style::window::default_body_width,
                                    style::window::calendar::item::dayEvents::box_height);
        description->setMargins(gui::Margins(style::margins::small, style::margins::big, 0, style::margins::very_big));
        description->setEdges(gui::RectangleEdge::None);
        description->setFont(style::window::font::medium);
        description->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void DayEventsItem::setEvent(std::shared_ptr<EventsRecord> rec)
    {
        this->record = rec;

        if (rec != nullptr) {
            description->setText(this->record->title.c_str());
            title->setText(utils::time::TimeRangeParser().getCalendarTimeString(record->date_from, record->date_till));
            if (record->reminder == static_cast<uint32_t>(Reminder::never)) {
                clock->setVisible(false);
            }
        }
    }
} /* namespace gui */
