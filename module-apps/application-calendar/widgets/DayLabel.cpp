// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DayLabel.hpp"
#include "MonthBox.hpp"
#include <application-calendar/ApplicationCalendar.hpp>
#include <application-calendar/data/CalendarData.hpp>
#include <application-calendar/widgets/CalendarStyle.hpp>

#include <gui/widgets/Image.hpp>
#include <time/time_conversion.hpp>
#include <gui/widgets/Label.hpp>

namespace gui
{
    DayLabel::DayLabel(app::ApplicationCommon *app,
                       gui::Item *parent,
                       const uint32_t &cellIndex,
                       const uint32_t &firstWeekOffset,
                       const uint32_t &width,
                       const uint32_t &height,
                       bool isDayEmpty)
        : Rect(parent, 0, 0, width, height)
    {
        this->setEdges(RectangleEdge::None);

        this->vBox = new gui::VBox(this, 0, 0, 0, 0);
        this->vBox->setEdges(gui::RectangleEdge::None);

        this->dayNumber = new gui::Label(this->vBox, 0, 0, 0, 0);
        this->dayNumber->setMinimumSize(width, height - 22);
        this->dayNumber->setEdges(RectangleEdge::None);
        this->dayNumber->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        this->dayNumber->setMargins(gui::Margins(0, 11, 0, 0));
        this->dayNumber->activeItem = false;

        this->dotImage = new gui::Image(this, width / 2 - 3, height - 12, 0, 0, "calendar_event_dot");
        this->dotImage->setMinimumSize(11, 11);
        this->dotImage->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        this->dotImage->setVisible(false);

        if (cellIndex < style::window::calendar::week_days_number) {
            this->dayNumber->setText(utils::time::Locale::get_short_day(cellIndex));
            this->dayNumber->setFont(style::window::font::verysmall);
            this->activeItem = false;
        }
        else if (cellIndex >= style::window::calendar::week_days_number &&
                 cellIndex < style::window::calendar::week_days_number + firstWeekOffset) {
            this->setPenWidth(style::window::default_border_no_focus_w);
            this->activeItem = false;
        }
        else {
            auto actualMonthBox = dynamic_cast<gui::MonthBox *>(parent);
            assert(actualMonthBox != nullptr);

            uint32_t numb = cellIndex - firstWeekOffset - style::window::calendar::week_days_number + 1;
            date::year_month_day actualDate = TimePointToYearMonthDay(TimePointNow());
            std::string number              = std::to_string(numb);
            if (!isDayEmpty) {
                this->dotImage->setVisible(true);
            }
            this->dayNumber->setText(number);
            this->activeItem = true;
            if (numb == unsigned(actualDate.day()) &&
                (actualMonthBox->monthFilterValue.month() == actualDate.month()) &&
                actualMonthBox->monthFilterValue.year() == actualDate.year()) {
                this->dayNumber->setFont(style::window::font::mediumbold);
            }
            else {
                this->dayNumber->setFont(style::window::font::medium);
            }
            this->activatedCallback = [=](gui::Item &item) {
                return true;
            };
            this->setPenWidth(style::window::default_border_no_focus_w);
            this->setPenFocusWidth(style::window::default_border_focus_w);
            this->setEdges(RectangleEdge::Top | RectangleEdge::Bottom);
        }

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    uint32_t DayLabel::getDayNumber()
    {
        std::string text = dayNumber->getText();
        try {
            auto result = std::stoi(text.c_str());
            if (result == 0 || result > max_month_day) {
                LOG_ERROR("Wrong day number!");
                return 0;
            }
            return result;
        }
        catch (std::exception &e) {
            LOG_ERROR("DayLabel::getDayNumber: %s", e.what());
            return 0;
        }
    }

} /* namespace gui */
