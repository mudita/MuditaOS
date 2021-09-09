// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DayLabel.hpp"
#include "MonthBox.hpp"
#include <application-calendar/widgets/CalendarStyle.hpp>
#include <windows/CalendarMainWindow.hpp>

namespace gui
{
    MonthBox::MonthBox(app::ApplicationCommon *app,
                       gui::Item *parent,
                       const int &offsetTop,
                       const uint32_t &width,
                       const uint32_t &height,
                       const uint32_t &dayWidth,
                       const uint32_t &dayHeight,
                       const std::unique_ptr<MonthModel> &model,
                       std::array<bool, 31> &isDayEmpty)
        : GridLayout(parent, style::window::default_left_margin, offsetTop, width, height, {dayWidth, dayHeight})
    {
        assert(parent);
        parent->addWidget(this);
        month            = model->getMonthText();
        monthFilterValue = model->getYear() / model->getMonth();
        grid.x           = dayWidth;
        grid.y           = dayHeight;

        uint32_t firstDayOffset = model->getFirstWeekOffset();
        uint32_t lastDay        = model->getLastDay();
        uint32_t iterations     = style::window::calendar::week_days_number + firstDayOffset + lastDay;

        uint32_t i;
        for (i = 0; i < iterations; ++i) {
            if (i < style::window::calendar::week_days_number + firstDayOffset) {
                auto day = new DayLabel(app,
                                        this,
                                        i,
                                        firstDayOffset,
                                        style::window::calendar::day_cell_width,
                                        style::window::calendar::day_cell_height,
                                        true);
                addWidget(day);
            }
            else {
                auto day = new DayLabel(app,
                                        this,
                                        i,
                                        firstDayOffset,
                                        style::window::calendar::day_cell_width,
                                        style::window::calendar::day_cell_height,
                                        isDayEmpty[i - (style::window::calendar::week_days_number + firstDayOffset)]);
                addWidget(day);
            }
        }
        date::year_month_day actualDate = TimePointToYearMonthDay(TimePointNow());
        if (model->getYear() == actualDate.year() && model->getMonth() == actualDate.month()) {
            focusChangedCallback = [=](Item &item) {
                setFocusOnElement(unsigned(actualDate.day()) - 1);
                return true;
            };
        }
    }
} /* namespace gui */
