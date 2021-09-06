// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateWidget.hpp"
#include "DateAndTimeStyle.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>
#include <time/time_date_validation.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <time/dateCommon.hpp>

namespace date_and_time = style::window::date_and_time;

namespace gui
{
    DateWidget::DateWidget(Item *parent) : VBox(parent)
    {
        setMinimumSize(style::window::default_body_width, date_and_time::height);
        setEdges(RectangleEdge::None);

        buildInterface();
        applyCallbacks();
    }

    void DateWidget::buildInterface()
    {
        labelsHBox = new HBox(this, 0, 0, 0, 0);
        labelsHBox->setMinimumSize(style::window::default_body_width, date_and_time::topMargin);
        labelsHBox->setMargins(Margins(0, 0, 0, date_and_time::topMargin / 4));
        labelsHBox->setEdges(RectangleEdge::None);
        labelsHBox->activeItem = false;

        dayLabel = new Label(labelsHBox, 0, 0, 0, 0);
        applyLabelSpecificProperties(dayLabel);
        dayLabel->setText(utils::translate("app_settings_title_day"));

        monthLabel = new Label(labelsHBox, 0, 0, 0, 0);
        applyLabelSpecificProperties(monthLabel);
        monthLabel->setMargins(Margins(date_and_time::separator, 0, 0, 0));
        monthLabel->setText(utils::translate("app_settings_title_month"));

        yearLabel = new Label(labelsHBox, 0, 0, 0, 0);
        applyLabelSpecificProperties(yearLabel);
        yearLabel->setMargins(Margins(date_and_time::separator, 0, 0, 0));
        yearLabel->setText(utils::translate("app_settings_title_year"));

        dateHBox = new HBox(this, 0, 0, 0, 0);
        dateHBox->setMinimumSize(style::window::default_body_width, date_and_time::hBox_h);
        dateHBox->setEdges(RectangleEdge::None);
        dateHBox->activeItem = false;

        dayInput = new Label(dateHBox, 0, 0, 0, 0);

        monthInput = new Label(dateHBox, 0, 0, 0, 0);
        monthInput->setMargins(Margins(date_and_time::separator, 0, 0, 0));

        yearInput = new Label(dateHBox, 0, 0, 0, 0);
        yearInput->setMargins(Margins(date_and_time::separator, 0, 0, 0));

        applyItemSpecificProperties(dayInput);
        applyItemSpecificProperties(monthInput);
        applyItemSpecificProperties(yearInput);

        resizeItems();
    }

    void DateWidget::applyItemSpecificProperties(Label *item)
    {
        item->setMinimumSize(date_and_time::time_input_12h_w, date_and_time::hBox_h);
        item->setEdges(RectangleEdge::Bottom);
        item->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        item->setFont(style::window::font::largelight);
        item->setPenFocusWidth(style::window::default_border_focus_w);
        item->setPenWidth(style::window::default_border_rect_no_focus);
    }

    void DateWidget::applyLabelSpecificProperties(Label *label)
    {
        label->setMinimumSize(date_and_time::time_input_12h_w, date_and_time::topMargin);
        label->setEdges(RectangleEdge::None);
        label->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        label->setFont(style::window::font::small);
        label->activeItem = false;
    }

    void DateWidget::applyCallbacks()
    {
        focusChangedCallback = [&]([[maybe_unused]] Item &item) {
            setFocusItem(focus ? dateHBox : nullptr);
            return true;
        };

        inputCallback = [&](Item &item, const InputEvent &event) {
            auto focusedItem = getFocusItem();
            if (!event.isShortRelease()) {
                return false;
            }
            if (event.is(KeyCode::KEY_ENTER) || event.is(KeyCode::KEY_RF)) {
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

        setOnInputCallback(*dayInput);
        setOnInputCallback(*monthInput);
        setOnInputCallback(*yearInput);
    }

    date::year_month_day DateWidget::validateDate()
    {
        auto actualDate = TimePointToYearMonthDay(TimePointNow());
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

        if (year > utils::time::Locale::max_years) {
            yearInput->setText(std::to_string(utils::time::Locale::max_years));
        }

        year = std::clamp(year, utils::time::Locale::min_years, utils::time::Locale::max_years);

        if (month > static_cast<unsigned>(date::dec)) {
            monthInput->setText(std::to_string(static_cast<unsigned>(date::dec)));
        }
        month = std::clamp(static_cast<unsigned>(month), 1u, static_cast<unsigned>(date::dec));

        date::year_month_day_last max_date = date::year(year) / date::month(month) / date::last;
        if (day > static_cast<unsigned>(max_date.day())) {
            dayInput->setText(std::to_string(static_cast<unsigned>(max_date.day())));
        }
        day = std::clamp(static_cast<unsigned>(day), 1u, static_cast<unsigned>(max_date.day()));

        return date::year(year) / date::month(month) / date::day(day);
    }

    const date::year_month_day DateWidget::getChosenDate()
    {
        return validateDate();
    }

    void DateWidget::setDate(int keyValue, Label &item)
    {
        auto itemValue = item.getText();
        auto key       = std::to_string(keyValue);
        if (itemValue == "0") {
            itemValue = key;
        }
        else {
            itemValue += key;
        }
        item.setText(itemValue);

        if (!utils::time::validateDate(dayInput->getText(), monthInput->getText(), yearInput->getText())) {
            item.setText(key);
        }
    }

    void DateWidget::setOnInputCallback(Label &dateInput)
    {
        dateInput.inputCallback = [&](Item &item, const InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }
            if (event.isDigit()) {
                setDate(event.numericValue(), dateInput);
                return true;
            }
            else if (event.is(KeyCode::KEY_PND)) {
                clearInput(dateInput);
                return true;
            }
            return false;
        };
    }

    void DateWidget::clearInput(Label &dateInput)
    {
        auto value = dateInput.getText();
        if (auto length = value.length(); length > 0) {
            value.removeChar(length - 1);
            dateInput.setText(value);
        }
    }

    void DateWidget::loadData(const date::year_month_day &yearMonthDay)
    {
        dayInput->setText(std::to_string(static_cast<unsigned>(yearMonthDay.day())));
        monthInput->setText(std::to_string(static_cast<unsigned>(yearMonthDay.month())));
        yearInput->setText(std::to_string(static_cast<int>(yearMonthDay.year())));
    }

} /* namespace gui */
