// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventTimeItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>
#include <time/time_date_validation.hpp>

namespace gui
{
    namespace timeItem = style::window::calendar::item::eventTime;

    EventTimeItem::EventTimeItem(const std::string &description,
                                 DateTimeType dateTimeType,
                                 bool mode24H,
                                 std::function<void(const UTF8 &text)> bottomBarTemporaryMode,
                                 std::function<void()> bottomBarRestoreFromTemporaryMode)
        : dateTimeType(dateTimeType), mode24H{mode24H}, bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        setMinimumSize(style::window::default_body_width, timeItem::height);

        setEdges(RectangleEdge::None);
        setMargins(gui::Margins(style::window::calendar::leftMargin, timeItem::margin, 0, 0));

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdge::None);
        vBox->activeItem = false;

        descriptionLabel = new gui::Label(vBox, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::window::default_body_width, timeItem::margin);
        descriptionLabel->setMargins(gui::Margins(0, 0, 0, timeItem::margin / 4));
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;

        hBox = new gui::HBox(vBox, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width, timeItem::hBox_h);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->activeItem = false;

        hourInput = new gui::Label(hBox, 0, 0, 0, 0);
        hourInput->setEdges(gui::RectangleEdge::Bottom);
        hourInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        hourInput->setFont(style::window::font::largelight);
        hourInput->setPenFocusWidth(style::window::default_border_focus_w);
        hourInput->setPenWidth(style::window::default_border_rect_no_focus);

        colonLabel = new gui::Label(hBox, 0, 0, 0, 0);
        colonLabel->setMinimumSize(timeItem::separator, timeItem::hBox_h);
        colonLabel->setEdges(gui::RectangleEdge::None);
        colonLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        colonLabel->setFont(style::window::font::medium);
        colonLabel->setText(":");
        colonLabel->activeItem = false;

        minuteInput = new gui::Label(hBox, 0, 0, 0, 0);
        minuteInput->setEdges(gui::RectangleEdge::Bottom);
        minuteInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        minuteInput->setFont(style::window::font::largelight);
        minuteInput->setPenFocusWidth(style::window::default_border_focus_w);
        minuteInput->setPenWidth(style::window::default_border_rect_no_focus);

        descriptionLabel->setText(description);

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? hBox : nullptr);
            if (!item.focus) {
                validateHour();
            }
            return true;
        };

        applyInputCallbacks();
        prepareForTimeMode();
    }

    void EventTimeItem::applyInputCallbacks()
    {
        inputCallback = [&](Item &item, const InputEvent &event) {
            auto focusedItem = getFocusItem();
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (event.keyCode == gui::KeyCode::KEY_ENTER || event.keyCode == gui::KeyCode::KEY_RF) {
                return false;
            }

            if (focusedItem->onInput(event)) {
                uint32_t hours;
                try {
                    hours = std::stoi(hourInput->getText().c_str());
                }
                catch (std::exception &e) {
                    LOG_ERROR("EventTimeItem::applyInputCallbacks hours: %s", e.what());
                    hours = 0;
                }
                uint32_t minutes;
                try {
                    minutes = std::stoi(minuteInput->getText().c_str());
                }
                catch (std::exception &e) {
                    LOG_ERROR("EventTimeItem::applyInputCallbacks minutes: %s", e.what());
                    minutes = 0;
                }

                if (mode24H && hours > style::window::calendar::time::max_hour_24H_mode) {
                    hourInput->setText("0");
                }
                else if (!mode24H && hours > style::window::calendar::time::max_hour_12H_mode) {
                    hourInput->setText("12");
                }
                if (minutes > style::window::calendar::time::max_minutes) {
                    minuteInput->setText("0");
                }
                return true;
            }
            else if (hBox->onInput(event)) {
                return true;
            }

            return false;
        };

        onSaveCallback = [&](std::shared_ptr<EventsRecord> record) {
            validateHour();
            auto hours   = std::chrono::hours(std::stoi(hourInput->getText().c_str()));
            auto minutes = std::chrono::minutes(std::stoi(minuteInput->getText().c_str()));
            if (!mode24H) {
                hours = date::make24(hours, isPm(mode12hInput->getText()));
            }
            if (dateTimeType == DateTimeType::Start) {
                record->date_from =
                    TimePointFromYearMonthDay(TimePointToYearMonthDay(record->date_from)) + hours + minutes;
            }
            else {
                record->date_till =
                    TimePointFromYearMonthDay(TimePointToYearMonthDay(record->date_till)) + hours + minutes;
            }
        };

        onInputCallback(*hourInput);
        onInputCallback(*minuteInput);
    }

    void EventTimeItem::prepareForTimeMode()
    {
        if (!mode24H) {
            mode12hInput = new gui::Label(hBox, 0, 0, 0, 0);
            mode12hInput->setEdges(gui::RectangleEdge::Bottom);
            mode12hInput->setAlignment(
                gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            mode12hInput->setFont(style::window::font::largelight);
            mode12hInput->setPenFocusWidth(style::window::default_border_focus_w);
            mode12hInput->setPenWidth(style::window::default_border_rect_no_focus);
            mode12hInput->setText(utils::time::Locale::getAM());
            mode12hInput->inputCallback = [&](Item &item, const InputEvent &event) {
                if (event.state != gui::InputEvent::State::keyReleasedShort) {
                    return false;
                }
                if (event.keyCode == gui::KeyCode::KEY_LF) {
                    if (mode12hInput->getText() == utils::time::Locale::getAM()) {
                        mode12hInput->setText(utils::time::Locale::getPM());
                    }
                    else {
                        mode12hInput->setText(utils::time::Locale::getAM());
                    }
                    return true;
                }
                return false;
            };
            mode12hInput->focusChangedCallback = [&](Item &item) {
                if (item.focus) {
                    bottomBarTemporaryMode(utils::localize.get("common_switch"));
                }
                else {
                    bottomBarRestoreFromTemporaryMode();
                }
                return true;
            };

            mode12hInput->setMinimumSize(timeItem::time_input_12h_w, timeItem::hBox_h);
            mode12hInput->setMargins(gui::Margins(timeItem::separator, 0, 0, 0));
            hourInput->setMinimumSize(timeItem::time_input_12h_w, timeItem::hBox_h);
            minuteInput->setMinimumSize(timeItem::time_input_12h_w, timeItem::hBox_h);

            onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
                if (dateTimeType == DateTimeType::Start) {
                    auto start_time = TimePointToHourMinSec(event->date_from);

                    hourInput->setText(TimePointToHourString12H(event->date_from));
                    minuteInput->setText(TimePointToMinutesString(event->date_from));
                    setMode12hInput(start_time.hours());
                }
                else {
                    auto end_time = TimePointToHourMinSec(event->date_till);

                    hourInput->setText(TimePointToHourString12H(event->date_till));
                    minuteInput->setText(TimePointToMinutesString(event->date_till));
                    setMode12hInput(end_time.hours());
                }
            };
        }
        else {
            hourInput->setMinimumSize(timeItem::time_input_24h_w, timeItem::hBox_h);
            minuteInput->setMinimumSize(timeItem::time_input_24h_w, timeItem::hBox_h);

            onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
                if (dateTimeType == DateTimeType::Start) {
                    auto start_time = TimePointToHourMinSec(event->date_from);
                    hourInput->setText(std::to_string(start_time.hours().count()));
                    minuteInput->setText(std::to_string(start_time.minutes().count()));
                }
                else {
                    auto end_time = TimePointToHourMinSec(event->date_till);
                    hourInput->setText(std::to_string(end_time.hours().count()));
                    minuteInput->setText(std::to_string(end_time.minutes().count()));
                }
            };
        }
    }

    bool EventTimeItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }

    bool EventTimeItem::isPm(const UTF8 &text)
    {
        return !(text == utils::time::Locale::getAM());
    }

    void EventTimeItem::validateHour()
    {
        if (!utils::time::validateTime(hourInput->getText(), minuteInput->getText(), !mode24H)) {
            hourInput->setText("0");
            minuteInput->setText("0");
        }
    }

    void EventTimeItem::setTime(int keyValue, gui::Label &item)
    {
        auto itemValue = item.getText();
        auto key       = std::to_string(keyValue);
        if (itemValue == "0" || itemValue == "00") {
            itemValue = key;
        }
        else {
            itemValue += key;
        }
        item.setText(itemValue);

        if (!utils::time::validateTime(hourInput->getText(), minuteInput->getText(), !mode24H)) {
            item.setText(key);
        }
    }

    void EventTimeItem::onInputCallback(gui::Label &timeInput)
    {
        timeInput.inputCallback = [&](Item &item, const InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (auto value = gui::toNumeric(event.keyCode); value >= 0) {
                setTime(value, timeInput);
                return true;
            }
            else if (event.is(gui::KeyCode::KEY_PND)) {
                clearInput(timeInput);
                return true;
            }
            return false;
        };
    }

    void EventTimeItem::clearInput(gui::Label &timeInput)
    {
        if (auto length = timeInput.getText().length(); length > 0) {
            auto value = timeInput.getText();
            value.removeChar(length - 1);
            if (value.length() == 0) {
                value = "0";
            }
            timeInput.setText(value);
        }
    }

    void EventTimeItem::setMode12hInput(std::chrono::hours hours)
    {
        if (date::is_am(hours)) {
            mode12hInput->setText(utils::time::Locale::getAM());
        }
        else {
            mode12hInput->setText(utils::time::Locale::getPM());
        }
    }

} /* namespace gui */
