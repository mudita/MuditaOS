// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsDateItem.hpp"

#include <InputEvent.hpp>
#include <widgets/DateAndTimeStyle.hpp>
#include <widgets/WidgetsUtils.hpp>

namespace date_and_time = style::window::date_and_time;

namespace gui
{
    SettingsDateItem::SettingsDateItem()
    {
        setMinimumSize(style::window::default_body_width, date_and_time::height);
        setEdges(RectangleEdge::None);
        setMargins(Margins(date_and_time::leftMargin, date_and_time::topMargin, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);

        dateWidget = new DateWidget(vBox);

        onLoadCallback = [&](std::shared_ptr<utils::time::FromTillDate> fromTillDate) {
            dateWidget->loadData(TimePointToYearMonthDay(fromTillDate->from));
        };

        inputCallback = [&](Item &item, const InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }
            return vBox->onInput(event);
        };
        passDefaultCallbacksFromListItem(this, vBox);
    }
} /* namespace gui */
