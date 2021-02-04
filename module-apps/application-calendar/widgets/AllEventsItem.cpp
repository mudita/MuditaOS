// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AllEventsItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <Style.hpp>
#include <gui/widgets/Label.hpp>
#include <time/time_conversion.hpp>
#include <module-utils/time/TimeRangeParser.hpp>

namespace gui
{
    AllEventsItem::AllEventsItem()
    {
        setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        setMargins(gui::Margins(0, style::margins::big, 0, 0));

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);

        startTime = new gui::Label(hBox, 0, 0, 0, 0);
        startTime->setMinimumSize(style::window::calendar::item::allEvents::start_time_min_w,
                                  style::window::label::big_h);
        startTime->setMaximumSize(style::window::default_body_width, style::window::label::big_h);
        startTime->setEdges(gui::RectangleEdge::None);
        startTime->setFont(style::window::font::small);
        startTime->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        startTime->setMargins(gui::Margins(style::margins::small, 0, 0, 0));

        description = new gui::Label(hBox, 0, 0, 0, 0);
        description->setMinimumSize(style::window::calendar::item::allEvents::description_w,
                                    style::window::label::big_h);
        description->setEdges(gui::RectangleEdge::None);
        description->setFont(style::window::font::bigbold);
        description->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
    }

    bool AllEventsItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);
        return true;
    }

    void AllEventsItem::setMarkerItem(UTF8 text)
    {
        hBox->erase();

        auto image = new gui::Image();
        image->set("Rectangle");
        image->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        auto dayMonthLabel = new gui::Label();
        dayMonthLabel->setMinimumSize(style::window::calendar::item::allEvents::start_time_min_w,
                                      style::window::label::big_h);
        dayMonthLabel->setMaximumSize(style::window::default_body_width, style::window::label::big_h);
        dayMonthLabel->setEdges(RectangleEdge::None);
        dayMonthLabel->setFont(style::window::font::small);
        dayMonthLabel->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        dayMonthLabel->setMargins(gui::Margins(style::margins::small, 0, 0, 0));
        dayMonthLabel->setText(text);

        hBox->addWidget(image);
        hBox->addWidget(dayMonthLabel);
        activeItem = false;
        setEdges(RectangleEdge::None);
    }

    UTF8 AllEventsItem::getLabelMarker() const
    {
        auto startDate = TimePointToYearMonthDay(record->date_from);

        std::string monthStr =
            utils::time::Locale::get_month(utils::time::Locale::Month(unsigned(startDate.month()) - 1));
        return std::to_string(unsigned(startDate.day())) + " " + monthStr;
    }

    void AllEventsItem::setEvent(std::shared_ptr<EventsRecord> rec)
    {
        this->record = rec;
        if (rec != nullptr) {
            description->setText(this->record->title.c_str());
            startTime->setText(utils::time::TimeRangeParser().getCalendarTimeString(
                record->date_from, record->date_till, utils::time::Version::abbrev));
        }
    }
} /* namespace gui */
