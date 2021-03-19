// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarDateItem.hpp"
#include "CalendarTimeItem.hpp"
#include <module-apps/widgets/DateAndTimeStyle.hpp>
#include <module-apps/widgets/WidgetsUtils.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace date_and_time = style::window::date_and_time;

namespace gui
{
    CalendarTimeItem::CalendarTimeItem(const std::string &description,
                                       TimeWidget::Type type,
                                       const std::function<void(const UTF8 &text)> &bottomBarTemporaryMode,
                                       const std::function<void()> &bottomBarRestoreFromTemporaryMode,
                                       TimePoint dateFrom,
                                       TimePoint dateTill)
    {
        setMinimumSize(style::window::default_body_width, date_and_time::height);
        setEdges(RectangleEdge::None);
        setMargins(gui::Margins(date_and_time::leftMargin, date_and_time::topMargin, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);

        timeWidget = new TimeWidget(vBox, description, type, bottomBarTemporaryMode, bottomBarRestoreFromTemporaryMode);
        timeWidget->loadData(std::chrono::hours(TimePointToHour24H(dateFrom)),
                             std::chrono::minutes(TimePointToMin(dateFrom)),
                             std::chrono::hours(TimePointToHour24H(dateTill)),
                             std::chrono::minutes(TimePointToMin(dateTill)));

        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            timeWidget->loadData(std::chrono::hours(TimePointToHour24H(event->date_from)),
                                 std::chrono::minutes(TimePointToMin(event->date_from)),
                                 std::chrono::hours(TimePointToHour24H(event->date_till)),
                                 std::chrono::minutes(TimePointToMin(event->date_till)));
        };

        onSaveCallback = [&](std::shared_ptr<EventsRecord> record) -> bool {
            auto fromTillDate = std::make_shared<utils::time::FromTillDate>(record->date_from, record->date_till);
            if (timeWidget->saveData(fromTillDate)) {
                record->date_from = fromTillDate->from;
                record->date_till = fromTillDate->till;
                return true;
            }
            return false;
        };
        passDefaultCallbacksFromListItem(this, vBox);
    }

    auto CalendarTimeItem::getFromTillDate() const -> std::shared_ptr<utils::time::FromTillDate>
    {
        auto fromTillDate = std::make_shared<utils::time::FromTillDate>();
        timeWidget->saveData(fromTillDate);
        return fromTillDate;
    }

    void CalendarTimeItem::setConnectionToSecondItem(CalendarTimeItem *item)
    {
        timeWidget->setConnectionToSecondItem(item->timeWidget);
    }

    void CalendarTimeItem::setConnectionToDateItem(CalendarDateItem *item)
    {
        timeWidget->setConnectionToDateItem(item->getDateWidget());
    }
} /* namespace gui */
