// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventDateItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>
#include <module-utils/date/include/date/date.h>

namespace gui
{

    EventDateItem::EventDateItem()
    {
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::eventTime::height);
        setEdges(RectangleEdge::None);
        setMargins(gui::Margins(style::margins::small, style::window::calendar::item::eventTime::margin, 0, 0));

        buildInterface();
        applyCallbacks();
    }

    void EventDateItem::buildInterface()
    {
        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);
        vBox->activeItem = false;

        labelsHBox = new gui::HBox(vBox, 0, 0, 0, 0);
        labelsHBox->setMinimumSize(style::window::default_body_width,
                                   style::window::calendar::item::eventTime::separator);
        labelsHBox->setEdges(RectangleEdge::None);
        labelsHBox->activeItem = false;

        dayLabel = new gui::Label(labelsHBox, 0, 0, 0, 0);
        dayLabel->setMinimumSize(style::window::calendar::item::eventTime::time_input_12h_w,
                                 style::window::calendar::item::eventTime::separator);
        dayLabel->setEdges(RectangleEdge::None);
        dayLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        dayLabel->setFont(style::window::font::small);
        dayLabel->activeItem = false;
        dayLabel->setText(utils::localize.get("app_settings_title_day"));

        monthLabel = new gui::Label(labelsHBox, 0, 0, 0, 0);
        monthLabel->setMinimumSize(style::window::calendar::item::eventTime::time_input_12h_w,
                                   style::window::calendar::item::eventTime::separator);
        monthLabel->setMargins(gui::Margins(style::window::calendar::item::eventTime::separator, 0, 0, 0));
        monthLabel->setEdges(RectangleEdge::None);
        monthLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        monthLabel->setFont(style::window::font::small);
        monthLabel->activeItem = false;
        monthLabel->setText(utils::localize.get("app_settings_title_month"));

        yearLabel = new gui::Label(labelsHBox, 0, 0, 0, 0);
        yearLabel->setMinimumSize(style::window::calendar::item::eventTime::time_input_12h_w,
                                  style::window::calendar::item::eventTime::separator);
        yearLabel->setMargins(gui::Margins(style::window::calendar::item::eventTime::separator, 0, 0, 0));
        yearLabel->setEdges(RectangleEdge::None);
        yearLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        yearLabel->setFont(style::window::font::small);
        yearLabel->activeItem = false;
        yearLabel->setText(utils::localize.get("app_settings_title_year"));

        dateHBox = new gui::HBox(vBox, 0, 0, 0, 0);
        dateHBox->setMinimumSize(style::window::default_body_width,
                                 style::window::calendar::item::eventTime::height -
                                     style::window::calendar::item::eventTime::separator);
        dateHBox->setEdges(RectangleEdge::None);
        dateHBox->activeItem = false;

        dayInput = new gui::Text(dateHBox, 0, 0, 0, 0);

        monthInput = new gui::Text(dateHBox, 0, 0, 0, 0);
        monthInput->setMargins(gui::Margins(style::window::calendar::item::eventTime::separator, 0, 0, 0));

        yearInput = new gui::Text(dateHBox, 0, 0, 0, 0);
        yearInput->setMargins(gui::Margins(style::window::calendar::item::eventTime::separator, 0, 0, 0));

        applyItemSpecificProperties(dayInput);
        applyItemSpecificProperties(monthInput);
        applyItemSpecificProperties(yearInput);
    }

    void EventDateItem::applyItemSpecificProperties(gui::Text *item)
    {
        item->setMinimumSize(style::window::calendar::item::eventTime::time_input_12h_w,
                             style::window::calendar::item::eventTime::height -
                                 style::window::calendar::item::eventTime::separator);
        item->setEdges(RectangleEdge::Bottom);
        item->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        item->setFont(style::window::font::largelight);
        item->setInputMode(new InputMode({InputMode::digit}));
        item->setPenFocusWidth(style::window::default_border_focus_w);
        item->setPenWidth(style::window::default_border_rect_no_focus);
        item->setEditMode(gui::EditMode::Edit);
    }

    void EventDateItem::applyCallbacks()
    {
        focusChangedCallback = [&]([[maybe_unused]] Item &item) {
            setFocusItem(focus ? dateHBox : nullptr);
            return true;
        };

        inputCallback = [&](Item &item, const InputEvent &event) {
            auto focusedItem = getFocusItem();
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (event.keyCode == gui::KeyCode::KEY_ENTER || event.keyCode == gui::KeyCode::KEY_RF) {
                return false;
            }
            if (focusedItem->onInput(event)) {
                validateDate();
                return true;
            }
            else if (dateHBox->onInput(event)) {
                return true;
            }
            return false;
        };

        yearInput->focusChangedCallback = [&](Item &item) {
            yearInput->setText(std::to_string(static_cast<int>(validateDate().year())));
            return true;
        };
        monthInput->focusChangedCallback = [&](Item &item) {
            monthInput->setText(std::to_string(static_cast<unsigned>(validateDate().month())));
            return true;
        };
        dayInput->focusChangedCallback = [&](Item &item) {
            dayInput->setText(std::to_string(static_cast<unsigned>(validateDate().day())));
            return true;
        };

        onLoadCallback = [&](std::shared_ptr<EventsRecord> record) {
            auto date = utils::time::CalendarConversion::TimePointToYearMonthDay(record->date_from);
            dayInput->setText(std::to_string(static_cast<unsigned>(date.day())));
            monthInput->setText(std::to_string(static_cast<unsigned>(date.month())));
            yearInput->setText(std::to_string(static_cast<int>(date.year())));
        };
    }

    bool EventDateItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }

    utils::time::YearMonthDay EventDateItem::validateDate()
    {
        auto actualDate = utils::time::CalendarConversion::TimePointToYearMonthDay(utils::time::TimePointNow());
        uint32_t day;
        try {
            day = std::stoi(dayInput->getText().c_str());
        }
        catch (std::exception &) {
            day = static_cast<unsigned>(actualDate.day());
        }
        uint32_t month;
        try {
            month = std::stoi(monthInput->getText().c_str());
        }
        catch (std::exception &) {
            month = static_cast<unsigned>(actualDate.month());
        }
        int year;
        try {
            year = std::stoi(yearInput->getText().c_str());
        }
        catch (std::exception &) {
            year = static_cast<int>(actualDate.year());
        }

        if (year > style::window::calendar::time::max_years) {
            yearInput->setText(std::to_string(style::window::calendar::time::max_years));
        }

        year = std::clamp(year, style::window::calendar::time::min_years, style::window::calendar::time::max_years);

        if (month > static_cast<unsigned>(date::dec)) {
            monthInput->setText(std::to_string(static_cast<unsigned>(date::dec)));
        }
        month = std::clamp(static_cast<unsigned>(month), 1u, static_cast<unsigned>(date::dec));

        utils::time::YearMonthDayLast max_date = date::year(year) / date::month(month) / date::last;
        if (day > static_cast<unsigned>(max_date.day())) {
            dayInput->setText(std::to_string(static_cast<unsigned>(max_date.day())));
        }
        day = std::clamp(static_cast<unsigned>(day), 1u, static_cast<unsigned>(max_date.day()));

        return date::year(year) / date::month(month) / date::day(day);
    }

    const utils::time::YearMonthDay EventDateItem::getChosenDate()
    {
        return validateDate();
    }

} /* namespace gui */
