#include "MonthBox.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/windows/CalendarMainWindow.hpp"
#include "application-calendar/widgets/DayLabel.hpp"

namespace gui
{
    MonthBox::MonthBox(app::Application *app,
                       gui::Item *parent,
                       const int &offsetTop,
                       const uint32_t &width,
                       const uint32_t &height,
                       const uint32_t &dayWidth,
                       const uint32_t &dayHeight,
                       const std::unique_ptr<MonthModel> &model,
                       bool *isDayEmpty)
        : GridLayout(parent, style::window::default_left_margin, offsetTop, width, height, {dayWidth, dayHeight})
    {
        LOG_DEBUG("Call MonthBox constructor");

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
        auto mainWindow = dynamic_cast<CalendarMainWindow *>(parent);
        if (mainWindow->returnedFromWindow) {
            focusChangedCallback = [=](Item &item) {
                YearMonthDay date = monthFilterValue.year() / monthFilterValue.month() / date::last;
                if (unsigned(date.day()) < mainWindow->dayFocusedBefore) {
                    setFocusOnElement(unsigned(date.day()) - 1);
                }
                else {
                    setFocusOnElement(mainWindow->dayFocusedBefore - 1);
                }
                return true;
            };
        }
        else {
            date::year_month_day actualDate = TimePointToYearMonthDay(TimePointNow());
            if (model->getYear() == actualDate.year() && model->getMonth() == actualDate.month()) {
                focusChangedCallback = [=](Item &item) {
                    setFocusOnElement(unsigned(actualDate.day()) - 1);
                    return true;
                };
            }
        }

        LOG_DEBUG("MonthBox constructor Completed Successfully!");
    }
} /* namespace gui */
