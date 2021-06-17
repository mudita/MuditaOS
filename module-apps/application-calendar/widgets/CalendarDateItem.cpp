// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarDateItem.hpp"
#include <apps-common/widgets/DateAndTimeStyle.hpp>
#include <apps-common/widgets/DateWidget.hpp>
#include <apps-common/widgets/WidgetsUtils.hpp>

namespace date_and_time = style::window::date_and_time;

namespace gui
{
    CalendarDateItem::CalendarDateItem()
    {
        setMinimumSize(style::window::default_body_width, date_and_time::height);
        setEdges(RectangleEdge::None);
        setMargins(Margins(date_and_time::leftMargin, date_and_time::topMargin, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);

        dateWidget = new DateWidget(vBox);

        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            dateWidget->loadData(TimePointToYearMonthDay(event->date_from));
        };

        passDefaultCallbacksFromListItem(this, vBox);
    }
} /* namespace gui */
