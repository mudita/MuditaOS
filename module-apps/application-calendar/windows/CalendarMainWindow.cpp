// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarMainWindow.hpp"

#include <application-calendar/ApplicationCalendar.hpp>
#include <application-calendar/widgets/CalendarStyle.hpp>

#include <gui/input/InputEvent.hpp>
#include <time/time_conversion.hpp>

namespace gui
{

    CalendarMainWindow::CalendarMainWindow(app::ApplicationCommon *app, const std::string &name)
        : AppWindow(app, name), app::AsyncCallbackReceiver{app}
    {
        std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
        this->actualDate = date::year_month_day{date::floor<date::days>(tp)};
        std::fill(std::begin(isDayEmpty), std::end(isDayEmpty), true);
        buildInterface();
    }

    void CalendarMainWindow::refresh()
    {
        erase(dateLabel);
        monthBox->erase();
        monthModel           = std::make_unique<MonthModel>(actualDate);
        std::string dateText = monthModel->getMonthYearText();
        this->buildMonth(monthModel);
        this->buildDateLabel(dateText);
    }

    void CalendarMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void CalendarMainWindow::buildMonth(std::unique_ptr<MonthModel> &model)
    {
        auto app = dynamic_cast<app::ApplicationCalendar *>(application);
        assert(app != nullptr);

        offsetFromTop = style::window::default_vertical_pos + style::window::calendar::month_year_height;
        monthWidth    = style::window::default_body_width;
        monthHeight   = style::window_height - style::window::default_vertical_pos - style::nav_bar::height;
        dayWidth      = style::window::calendar::day_cell_width;
        dayHeight     = style::window::calendar::day_cell_height;

        monthBox = new MonthBox(
            app, this, offsetFromTop, monthWidth, monthHeight, dayWidth, dayHeight, monthModel, isDayEmpty);
        addWidget(monthBox);

        monthBox->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (!inputEvent.isShortRelease()) {
                return false;
            }
            switch (inputEvent.getKeyCode()) {
            case KeyCode::KEY_UP: {
                decrementMonthInView();
                return true;
            }
            case KeyCode::KEY_DOWN: {
                incrementMonthInView();
                return true;
            }
            case KeyCode::KEY_LEFT: {
                auto it = std::prev(monthBox->getNavigationFocusedItem());
                if (it != monthBox->children.end() && (*it)->isActive()) {
                    monthBox->setFocusItem(*it);
                }
                else {
                    decrementMonthInView();
                    monthBox->setFocusOnLastElement();
                }
                return true;
            }
            case KeyCode::KEY_RIGHT: {
                auto it = std::next(monthBox->getNavigationFocusedItem());
                if (it != monthBox->children.end() && (*it)->isActive()) {
                    monthBox->setFocusItem(*it);
                }
                else {
                    incrementMonthInView();
                    monthBox->setFocusOnElement(0);
                }
                return true;
            }
            default: {
                return false;
            }
            }
        };

        setFocusItem(monthBox);
    }

    void CalendarMainWindow::decrementMonthInView()
    {
        if (actualDate.month() != date::January) {
            date::month prevMonth = --actualDate.month();
            actualDate            = actualDate.year() / prevMonth / date::last;
        }
        else {
            date::month prevMonth = date::December;
            date::year prevYear   = --actualDate.year();
            actualDate            = prevYear / prevMonth / date::last;
        }
        refresh();
        monthBox->setFocusOnLastElement();
    }

    void CalendarMainWindow::incrementMonthInView()
    {
        if (actualDate.month() != date::December) {
            date::month nextMonth = ++actualDate.month();
            actualDate            = actualDate.year() / nextMonth / 1;
        }
        else {
            date::month nextMonth = date::January;
            date::year nextYear   = ++actualDate.year();
            actualDate            = nextYear / nextMonth / 1;
        }
        refresh();
        monthBox->setFocusOnElement(0);
    }

    void CalendarMainWindow::buildDateLabel(std::string actualDateTime)
    {
        dateLabel = new Label(this,
                              style::window::default_left_margin,
                              style::window::default_vertical_pos,
                              style::window::default_body_width,
                              style::window::calendar::month_year_height,
                              actualDateTime);

        dateLabel->setPenWidth(style::window::default_border_no_focus_w);
        dateLabel->setFont(style::window::font::mediumbold);
        dateLabel->setAlignment(gui::Alignment(gui::Alignment::Vertical::Center));
        addWidget(dateLabel);
    }

    void CalendarMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_calendar_title_main"));

        monthModel = std::make_unique<MonthModel>(actualDate);
        this->buildMonth(monthModel);
        this->buildDateLabel(monthModel->getMonthYearText());

        navBar->setActive(gui::nav_bar::Side::Right, true);
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(style::strings::common::back));
    }

    void CalendarMainWindow::destroyInterface()
    {
        erase();
    }

    bool CalendarMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }
        return false;
    }

} // namespace gui
