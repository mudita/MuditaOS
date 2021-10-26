// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsDateItem.hpp"
#include "SettingsTimeItem.hpp"

#include <widgets/DateAndTimeStyle.hpp>
#include <widgets/WidgetsUtils.hpp>

namespace date_and_time = style::window::date_and_time;

namespace gui
{
    SettingsTimeItem::SettingsTimeItem(const std::string &description,
                                       TimeWidget::Type type,
                                       const std::function<void(const UTF8 &text)> &navBarTemporaryMode,
                                       const std::function<void()> &navBarRestoreFromTemporaryMode)
    {
        setMinimumSize(style::window::default_body_width, date_and_time::height);
        setEdges(RectangleEdge::None);
        setMargins(gui::Margins(date_and_time::leftMargin, date_and_time::topMargin, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);

        timeWidget = new TimeWidget(vBox, description, type, navBarTemporaryMode, navBarRestoreFromTemporaryMode);

        onLoadCallback = [&](std::shared_ptr<utils::time::FromTillDate> fromTillDate) {
            timeWidget->loadData(std::chrono::hours(TimePointToHour24H(fromTillDate->from)),
                                 std::chrono::minutes(TimePointToMin(fromTillDate->from)),
                                 std::chrono::hours(TimePointToHour24H(fromTillDate->till)),
                                 std::chrono::minutes(TimePointToMin(fromTillDate->till)));
        };

        onSaveCallback = [&](std::shared_ptr<utils::time::FromTillDate> fromTillDate) -> bool {
            return timeWidget->saveData(fromTillDate);
        };

        passDefaultCallbacksFromListItem(this, vBox);
    }

    void SettingsTimeItem::setConnectionToSecondItem(SettingsTimeItem *item)
    {
        timeWidget->setConnectionToSecondItem(item->timeWidget);
    }

    void SettingsTimeItem::setConnectionToDateItem(SettingsDateItem *item)
    {
        timeWidget->setConnectionToDateItem(item->getDateWidget());
    }
} /* namespace gui */
