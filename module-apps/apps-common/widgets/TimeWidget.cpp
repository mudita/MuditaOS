// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeWidget.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>
#include <time/time_date_validation.hpp>
#include "DateAndTimeStyle.hpp"
#include <time/TimeZone.hpp>
#include <service-time/api/TimeSettingsApi.hpp>

namespace gui
{
    namespace date_and_time = style::window::date_and_time;

    TimeWidget::TimeWidget(Item *parent,
                           const std::string &description,
                           Type type,
                           std::function<void(const UTF8 &text)> navBarTemporaryMode,
                           std::function<void()> navBarRestoreFromTemporaryMode)
        : VBox(parent), mode24H{!stm::api::isTimeFormat12h()}, type{type},
          navBarTemporaryMode(std::move(navBarTemporaryMode)),
          navBarRestoreFromTemporaryMode(std::move(navBarRestoreFromTemporaryMode))
    {
        setMinimumSize(style::window::default_body_width, date_and_time::height);
        setEdges(RectangleEdge::None);

        descriptionLabel = new gui::Label(this, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::window::default_body_width, date_and_time::topMargin);
        descriptionLabel->setMargins(gui::Margins(0, 0, 0, date_and_time::topMargin / 4));
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width, date_and_time::hBox_h);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->activeItem = false;

        hourInput = new gui::Label(hBox, 0, 0, 0, 0);
        hourInput->setEdges(gui::RectangleEdge::Bottom);
        hourInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        hourInput->setFont(style::window::font::largelight);
        hourInput->setPenFocusWidth(style::window::default_border_focus_w);
        hourInput->setPenWidth(style::window::default_border_rect_no_focus);

        colonLabel = new gui::Label(hBox, 0, 0, 0, 0);
        colonLabel->setMinimumSize(date_and_time::separator, date_and_time::hBox_h);
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

    void TimeWidget::applyInputCallbacks()
    {
        inputCallback = [&](Item &item, const InputEvent &event) {
            auto focusedItem = getFocusItem();
            if (!event.isShortRelease()) {
                return false;
            }
            if (event.is(gui::KeyCode::KEY_ENTER) || event.is(gui::KeyCode::KEY_RF)) {
                return false;
            }

            if (focusedItem->onInput(event)) {
                if (secondItem != nullptr) {
                    uint32_t hours;

                    try {
                        hours = std::stoi(hourInput->getText().c_str());
                    }
                    catch (std::exception &e) {
                        LOG_ERROR("applyInputCallbacks hours: %s", e.what());
                        return true;
                    }

                    auto autofill_hour = hours + 1;
                    if (type == Type::Start && !mode24H) {
                        if (mode12hInput->getText() == timeConstants::after_noon) {
                            if (autofill_hour == utils::time::Locale::max_hour_12H_mode) {
                                autofill_hour = utils::time::Locale::max_hour_12H_mode - 1;
                                secondItem->minuteInput->setText(std::to_string(utils::time::Locale::max_minutes));
                            }
                            else {
                                secondItem->minuteInput->setText(minuteInput->getText());
                            }
                            secondItem->mode12hInput->setText(mode12hInput->getText());
                        }
                        else {
                            if (autofill_hour == utils::time::Locale::max_hour_12H_mode) {
                                secondItem->mode12hInput->setText(timeConstants::after_noon);
                            }
                            secondItem->minuteInput->setText(minuteInput->getText());
                        }
                        if (autofill_hour > utils::time::Locale::max_hour_12H_mode) {
                            autofill_hour = 1;
                            secondItem->mode12hInput->setText(mode12hInput->getText());
                            secondItem->minuteInput->setText(minuteInput->getText());
                        }
                        secondItem->hourInput->setText(std::to_string(autofill_hour));
                    }
                    else if (type == Type::Start && mode24H) {
                        if (secondItem != nullptr) {
                            secondItem->minuteInput->setText(minuteInput->getText());
                            if (autofill_hour > utils::time::Locale::max_hour_24H_mode) {
                                autofill_hour = utils::time::Locale::max_hour_24H_mode;
                                secondItem->minuteInput->setText(minuteInput->getText());
                            }
                            secondItem->hourInput->setText(std::to_string(autofill_hour));
                        }
                    }
                }
                return true;
            }
            else if (hBox->onInput(event)) {
                return true;
            }

            return false;
        };

        onInputCallback(*hourInput);
        onInputCallback(*minuteInput);
    }

    void TimeWidget::prepareForTimeMode()
    {
        if (!mode24H) {
            prepareMode12HInputLabel();
            hourInput->setMinimumSize(date_and_time::time_input_12h_w, date_and_time::hBox_h);
            minuteInput->setMinimumSize(date_and_time::time_input_12h_w, date_and_time::hBox_h);
        }
        else {
            hourInput->setMinimumSize(date_and_time::time_input_24h_w, date_and_time::hBox_h);
            minuteInput->setMinimumSize(date_and_time::time_input_24h_w, date_and_time::hBox_h);
        }
    }

    void TimeWidget::prepareMode12HInputLabel()
    {
        mode12hInput = new gui::Label(hBox, 0, 0, 0, 0);
        mode12hInput->setEdges(gui::RectangleEdge::Bottom);
        mode12hInput->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        mode12hInput->setFont(style::window::font::largelight);
        mode12hInput->setPenFocusWidth(style::window::default_border_focus_w);
        mode12hInput->setPenWidth(style::window::default_border_rect_no_focus);
        mode12hInput->setText(timeConstants::before_noon);
        mode12hInput->inputCallback = [&](Item &item, const InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }
            if (event.is(gui::KeyCode::KEY_LF)) {
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
                navBarTemporaryMode(utils::translate("common_switch"));
            }
            else {
                navBarRestoreFromTemporaryMode();
            }
            return true;
        };

        mode12hInput->setMinimumSize(date_and_time::time_input_12h_w, date_and_time::hBox_h);
        mode12hInput->setMargins(gui::Margins(date_and_time::separator, 0, 0, 0));
    }

    void TimeWidget::setConnectionToSecondItem(gui::TimeWidget *item)
    {
        this->secondItem = item;
    }

    void TimeWidget::setConnectionToDateItem(gui::DateWidget *item)
    {
        this->dateItem = item;
    }

    bool TimeWidget::isPm(const std::string &text) const
    {
        return !(text == timeConstants::before_noon);
    }

    bool TimeWidget::validateHour() const
    {
        if (type == Type::End) {
            if (secondItem == nullptr) {
                LOG_ERROR("secondItem not connected!");
                return false;
            }
            std::chrono::hours start_hour;
            std::chrono::hours end_hour;
            uint32_t start_minutes;
            uint32_t end_minutes;
            try {
                auto hours = std::chrono::hours(std::stoi(secondItem->hourInput->getText().c_str()));
                if (!mode24H) {
                    start_hour = date::make24(hours, isPm(secondItem->mode12hInput->getText()));
                }
                else {
                    start_hour = hours;
                }
            }
            catch (std::exception &e) {
                LOG_WARN("start_hour: %s", e.what());
                return false;
            }

            try {
                auto hours = std::chrono::hours(std::stoi(hourInput->getText().c_str()));
                if (!mode24H) {
                    end_hour = date::make24(hours, isPm(mode12hInput->getText()));
                }
                else {
                    end_hour = hours;
                }
            }
            catch (std::exception &e) {
                LOG_ERROR("end_hour: %s", e.what());
                return false;
            }

            try {
                start_minutes = std::stoi(secondItem->minuteInput->getText().c_str());
            }
            catch (std::exception &e) {
                LOG_ERROR("start_minutes: %s", e.what());
                return false;
            }

            try {
                end_minutes = std::stoi(minuteInput->getText().c_str());
            }
            catch (std::exception &e) {
                LOG_ERROR("end_minutes: %s", e.what());
                return false;
            }

            if (!mode24H) {
                validateHourFor12hMode(start_hour, end_hour, start_minutes, end_minutes);
            }
            else {
                validateHourFor24hMode(start_hour, end_hour, start_minutes, end_minutes);
            }
        }
        return true;
    }

    void TimeWidget::validateHourFor12hMode(std::chrono::hours start_hour,
                                            std::chrono::minutes end_hour,
                                            uint32_t start_minutes,
                                            uint32_t end_minutes) const
    {
        if (start_hour > end_hour || (start_hour == end_hour && start_minutes > end_minutes)) {
            auto hour = start_hour.count() + 1;
            if (secondItem->mode12hInput->getText() == timeConstants::after_noon) {
                if (hour == utils::time::Locale::max_hour_12H_mode) {
                    hour = utils::time::Locale::max_hour_12H_mode - 1;
                    minuteInput->setText(std::to_string(utils::time::Locale::max_minutes));
                }
                else {
                    minuteInput->setText(secondItem->minuteInput->getText());
                }
                mode12hInput->setText(secondItem->mode12hInput->getText());
            }
            else {
                if (hour == utils::time::Locale::max_hour_12H_mode) {
                    mode12hInput->setText(timeConstants::after_noon);
                }
                minuteInput->setText(minuteInput->getText());
            }
            if (hour > utils::time::Locale::max_hour_12H_mode) {
                hour = 1;
                mode12hInput->setText(secondItem->mode12hInput->getText());
                minuteInput->setText(secondItem->minuteInput->getText());
            }
            hourInput->setText(utils::to_string(hour));
        }
    }

    void TimeWidget::validateHourFor24hMode(std::chrono::hours start_hour,
                                            std::chrono::minutes end_hour,
                                            uint32_t start_minutes,
                                            uint32_t end_minutes) const
    {
        if (start_hour > end_hour || (start_hour == end_hour && start_minutes > end_minutes)) {
            auto hour = start_hour.count() + 1;
            if (hour > utils::time::Locale::max_hour_24H_mode) {
                hour = utils::time::Locale::max_hour_24H_mode;
                minuteInput->setText(std::to_string(utils::time::Locale::max_minutes));
            }
            else {
                minuteInput->setText(secondItem->minuteInput->getText());
            }
            hourInput->setText(std::to_string(hour));
        }
    }

    void TimeWidget::setTime(int keyValue, gui::Label &item)
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

    void TimeWidget::onInputCallback(gui::Label &timeInput)
    {
        timeInput.inputCallback = [&](Item &item, const InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }
            if (event.isDigit()) {
                setTime(event.numericValue(), timeInput);
                return true;
            }
            else if (event.is(gui::KeyCode::KEY_PND)) {
                clearInput(timeInput);
                return true;
            }
            return false;
        };
    }

    void TimeWidget::clearInput(gui::Label &timeInput)
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

    void TimeWidget::loadData(const std::chrono::hours &hoursFrom,
                              const std::chrono::minutes &minutesFrom,
                              const std::chrono::hours &hoursTill,
                              const std::chrono::minutes &minutesTill)
    {
        if (!mode24H) {
            if (type == Type::Start) {
                const auto hours12H = date::make12(hoursFrom);
                hourInput->setText(utils::to_string(hours12H.count()));
                minuteInput->setText(utils::to_string(minutesFrom.count()));
                if (date::is_am(hoursFrom)) {
                    mode12hInput->setText(timeConstants::before_noon);
                }
                else {
                    mode12hInput->setText(timeConstants::after_noon);
                }
            }
            else if (type == Type::End) {
                const auto hours12H = date::make12(hoursTill);
                hourInput->setText(utils::to_string(hours12H.count()));
                minuteInput->setText(utils::to_string(minutesTill.count()));
                if (date::is_am(hoursTill)) {
                    mode12hInput->setText(timeConstants::before_noon);
                }
                else {
                    mode12hInput->setText(timeConstants::after_noon);
                }
            }
        }
        else {
            if (type == Type::Start) {
                hourInput->setText(utils::to_string(hoursFrom.count()));
                minuteInput->setText(utils::to_string(minutesFrom.count()));
            }
            else if (type == Type::End) {
                hourInput->setText(utils::to_string(hoursTill.count()));
                minuteInput->setText(utils::to_string(minutesTill.count()));
            }
        }
    }

    bool TimeWidget::saveData(std::shared_ptr<utils::time::FromTillDate> fromTillDate) const
    {
        if (!validateHour()) {
            return false;
        }
        auto hours   = std::chrono::hours(std::stoi(hourInput->getText().c_str()));
        auto minutes = std::chrono::minutes(std::stoi(minuteInput->getText().c_str()));

        if (!mode24H) {
            hours = date::make24(hours, isPm(mode12hInput->getText()));
        }
        auto date = (dateItem != nullptr) ? TimePointFromYearMonthDay(dateItem->getChosenDate())
                                          : TimePointFromYearMonthDay(TimePointToYearMonthDay(TimePointNow()));

        TimePoint presetPointTime = date + hours + minutes;
        auto timeZoneOffset =
            utils::time::getTimeZoneOffset(stm::api::getCurrentTimezoneName(), TimePointToTimeT(presetPointTime));

        if (type == Type::Start) {
            fromTillDate->from = presetPointTime - timeZoneOffset;
        }
        else if (type == Type::End) {
            fromTillDate->till = presetPointTime - timeZoneOffset;
        }

        return true;
    }

} /* namespace gui */
