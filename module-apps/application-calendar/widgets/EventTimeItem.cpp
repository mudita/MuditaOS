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
                                 bool mode24H,
                                 std::function<void(const UTF8 &text)> bottomBarTemporaryMode,
                                 std::function<void()> bottomBarRestoreFromTemporaryMode)
        : mode24H{mode24H}, bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
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

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
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
                    return true;
                }

                auto autofill_hour = hours + 1;
                if (this->descriptionLabel->getText() ==
                        utils::localize.get("app_calendar_new_edit_event_start").c_str() &&
                    !mode24H) {
                    if (mode12hInput->getText() == timeConstants::after_noon) {
                        if (autofill_hour == style::window::calendar::time::max_hour_12H_mode) {
                            autofill_hour = style::window::calendar::time::max_hour_12H_mode - 1;
                            secondItem->minuteInput->setText(
                                std::to_string(style::window::calendar::time::max_minutes));
                        }
                        else {
                            secondItem->minuteInput->setText(minuteInput->getText());
                        }
                        secondItem->mode12hInput->setText(mode12hInput->getText());
                    }
                    else {
                        if (autofill_hour == style::window::calendar::time::max_hour_12H_mode) {
                            secondItem->mode12hInput->setText(timeConstants::after_noon);
                        }
                        secondItem->minuteInput->setText(minuteInput->getText());
                    }
                    if (autofill_hour > style::window::calendar::time::max_hour_12H_mode) {
                        autofill_hour = 1;
                        secondItem->mode12hInput->setText(mode12hInput->getText());
                        secondItem->minuteInput->setText(minuteInput->getText());
                    }
                    secondItem->hourInput->setText(std::to_string(autofill_hour));
                }
                else if (this->descriptionLabel->getText() ==
                             utils::localize.get("app_calendar_new_edit_event_start").c_str() &&
                         mode24H) {
                    secondItem->minuteInput->setText(minuteInput->getText());
                    if (autofill_hour > style::window::calendar::time::max_hour_24H_mode) {
                        autofill_hour = style::window::calendar::time::max_hour_24H_mode;
                        secondItem->minuteInput->setText(minuteInput->getText());
                    }
                    secondItem->hourInput->setText(std::to_string(autofill_hour));
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
            if (this->descriptionLabel->getText() == utils::localize.get("app_calendar_new_edit_event_end")) {
                record->date_till = calculateEventTime(dateItem->getChosenDate(), hours, minutes);
            }
            else if (this->descriptionLabel->getText() == utils::localize.get("app_calendar_new_edit_event_start")) {
                record->date_from = calculateEventTime(dateItem->getChosenDate(), hours, minutes);
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
            mode12hInput->setText(timeConstants::before_noon);
            mode12hInput->inputCallback = [&](Item &item, const InputEvent &event) {
                if (event.state != gui::InputEvent::State::keyReleasedShort) {
                    return false;
                }
                if (event.keyCode == gui::KeyCode::KEY_LF) {
                    if (mode12hInput->getText() == timeConstants::before_noon) {
                        mode12hInput->setText(timeConstants::after_noon);
                    }
                    else {
                        mode12hInput->setText(timeConstants::before_noon);
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
                if (this->descriptionLabel->getText() == utils::localize.get("app_calendar_new_edit_event_start")) {
                    auto start_time = TimePointToHourMinSec(event->date_from);

                    hourInput->setText(TimePointToHourString12H(event->date_from));
                    minuteInput->setText(TimePointToMinutesString(event->date_from));

                    if (date::is_am(start_time.hours())) {
                        mode12hInput->setText(timeConstants::before_noon);
                    }
                    else {
                        mode12hInput->setText(timeConstants::after_noon);
                    }
                }
                else if (this->descriptionLabel->getText() == utils::localize.get("app_calendar_new_edit_event_end")) {
                    auto end_time = TimePointToHourMinSec(event->date_till);

                    hourInput->setText(TimePointToHourString12H(event->date_till));
                    minuteInput->setText(TimePointToMinutesString(event->date_till));
                    if (date::is_am(end_time.hours())) {
                        mode12hInput->setText(timeConstants::before_noon);
                    }
                    else {
                        mode12hInput->setText(timeConstants::after_noon);
                    }
                }
            };
        }
        else {
            hourInput->setMinimumSize(timeItem::time_input_24h_w, timeItem::hBox_h);
            minuteInput->setMinimumSize(timeItem::time_input_24h_w, timeItem::hBox_h);

            onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
                if (this->descriptionLabel->getText() == utils::localize.get("app_calendar_new_edit_event_start")) {
                    auto start_time = TimePointToHourMinSec(event->date_from);
                    hourInput->setText(std::to_string(start_time.hours().count()));
                    minuteInput->setText(std::to_string(start_time.minutes().count()));
                }
                else if (this->descriptionLabel->getText() == utils::localize.get("app_calendar_new_edit_event_end")) {
                    auto end_time = TimePointToHourMinSec(event->date_till);
                    hourInput->setText(std::to_string(end_time.hours().count()));
                    minuteInput->setText(std::to_string(end_time.minutes().count()));
                }
            };
        }
    }

    void EventTimeItem::setConnectionToSecondItem(gui::EventTimeItem *item)
    {
        this->secondItem = item;
    }

    void EventTimeItem::setConnectionToDateItem(gui::EventDateItem *item)
    {
        this->dateItem = item;
    }

    bool EventTimeItem::isPm(const std::string &text)
    {
        return !(text == timeConstants::before_noon);
    }

    void EventTimeItem::validateHour()
    {
        if (descriptionLabel->getText() == utils::localize.get("app_calendar_new_edit_event_end")) {
            std::chrono::hours start_hour;
            std::chrono::hours end_hour;
            uint32_t start_minutes;
            uint32_t end_minutes;
            try {
                start_hour = date::make24(std::chrono::hours(std::stoi(secondItem->hourInput->getText().c_str())),
                                          isPm(secondItem->mode12hInput->getText()));
            }
            catch (std::exception &e) {
                LOG_ERROR("EventTimeItem::validateHour start_hour: %s", e.what());
                start_hour = TimePointToHourMinSec(TimePointNow()).hours();
            }

            try {
                end_hour = date::make24(std::chrono::hours(std::stoi(hourInput->getText().c_str())),
                                        isPm(mode12hInput->getText()));
            }
            catch (std::exception &e) {
                LOG_ERROR("EventTimeItem::validateHour end_hour: %s", e.what());
                end_hour = start_hour + std::chrono::hours{1};
            }

            try {
                start_minutes = std::stoi(secondItem->minuteInput->getText().c_str());
            }
            catch (std::exception &e) {
                LOG_ERROR("EventTimeItem::validateHour start_minutes: %s", e.what());
                start_minutes = TimePointToHourMinSec(TimePointNow()).minutes().count();
            }

            try {
                end_minutes = std::stoi(minuteInput->getText().c_str());
            }
            catch (std::exception &e) {
                LOG_ERROR("EventTimeItem::validateHour end_minutes: %s", e.what());
                end_minutes = start_minutes;
            }

            if (!mode24H) {
                validateHourFor12hMode(start_hour, end_hour, start_minutes, end_minutes);
            }
            else {
                validateHourFor24hMode(start_hour, end_hour, start_minutes, end_minutes);
            }
        }
    }

    void EventTimeItem::validateHourFor12hMode(std::chrono::hours start_hour,
                                               std::chrono::minutes end_hour,
                                               uint32_t start_minutes,
                                               uint32_t end_minutes)
    {
        if (start_hour > end_hour || (start_hour == end_hour && start_minutes > end_minutes)) {
            auto hour = start_hour.count() + 1;
            if (secondItem->mode12hInput->getText() == timeConstants::after_noon) {
                if (hour == style::window::calendar::time::max_hour_12H_mode) {
                    hour = style::window::calendar::time::max_hour_12H_mode - 1;
                    minuteInput->setText(std::to_string(style::window::calendar::time::max_minutes));
                }
                else {
                    minuteInput->setText(secondItem->minuteInput->getText());
                }
                mode12hInput->setText(secondItem->mode12hInput->getText());
            }
            else {
                if (hour == style::window::calendar::time::max_hour_12H_mode) {
                    mode12hInput->setText(timeConstants::after_noon);
                }
                minuteInput->setText(minuteInput->getText());
            }
            if (hour > style::window::calendar::time::max_hour_12H_mode) {
                hour = 1;
                mode12hInput->setText(secondItem->mode12hInput->getText());
                minuteInput->setText(secondItem->minuteInput->getText());
            }
            hourInput->setText(utils::to_string(hour));
        }
    }

    void EventTimeItem::validateHourFor24hMode(std::chrono::hours start_hour,
                                               std::chrono::minutes end_hour,
                                               uint32_t start_minutes,
                                               uint32_t end_minutes)
    {
        if (start_hour > end_hour || (start_hour == end_hour && start_minutes > end_minutes)) {
            auto hour = start_hour.count() + 1;
            if (hour > style::window::calendar::time::max_hour_24H_mode) {
                hour = style::window::calendar::time::max_hour_24H_mode;
                minuteInput->setText(std::to_string(style::window::calendar::time::max_minutes));
            }
            else {
                minuteInput->setText(secondItem->minuteInput->getText());
            }
            hourInput->setText(std::to_string(hour));
        }
    }

    TimePoint EventTimeItem::calculateEventTime(calendar::YearMonthDay date,
                                                std::chrono::hours hours,
                                                std::chrono::minutes minutes)
    {
        return TimePointFromYearMonthDay(date) + hours + minutes;
    }

    void EventTimeItem::setTime(int keyValue, gui::Label &item)
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

} /* namespace gui */
