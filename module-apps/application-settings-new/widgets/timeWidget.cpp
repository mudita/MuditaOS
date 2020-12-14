// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "timeWidget.hpp"
#include "SettingsStyle.hpp"

#include <Style.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    TimeWidget::TimeWidget(Item *parent,
                           const std::string &description,
                           std::function<void(const UTF8 &text)> bottomBarTemporaryMode,
                           std::function<void()> bottomBarRestoreFromTemporaryMode)
        : VBox(parent, 0, 0, style::window::default_body_width, style::settings::widget::time::h),
          bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        setEdges(RectangleEdge::None);

        descriptionLabel = new gui::Label(
            this, 0, 0, style::window::default_body_width, style::settings::widget::time::description_label_h);
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;
        descriptionLabel->setText(description);

        hBox = new gui::HBox(this, 0, 0, style::window::default_body_width, style::settings::widget::time::body_h);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->activeItem = false;

        hourInput =
            new gui::Text(hBox, 0, 0, style::settings::widget::time::input_w, style::settings::widget::time::body_h);
        hourInput->setEdges(gui::RectangleEdge::Bottom);
        hourInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        hourInput->setFont(style::window::font::largelight);
        hourInput->setInputMode(new InputMode({InputMode::digit}));
        hourInput->setPenFocusWidth(style::window::default_border_focus_w);
        hourInput->setPenWidth(style::window::default_border_rect_no_focus);
        hourInput->setEditMode(gui::EditMode::Edit);

        colonLabel = new gui::Label(
            hBox, 0, 0, style::settings::widget::time::separator_w, style::settings::widget::time::body_h);
        colonLabel->setEdges(gui::RectangleEdge::None);
        colonLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        colonLabel->setFont(style::window::font::medium);
        colonLabel->setText(":");
        colonLabel->activeItem = false;

        minuteInput =
            new gui::Text(hBox, 0, 0, style::settings::widget::time::input_w, style::settings::widget::time::body_h);
        minuteInput->setEdges(gui::RectangleEdge::Bottom);
        minuteInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        minuteInput->setFont(style::window::font::largelight);
        minuteInput->setInputMode(new InputMode({InputMode::digit}));
        minuteInput->setPenFocusWidth(style::window::default_border_focus_w);
        minuteInput->setPenWidth(style::window::default_border_rect_no_focus);
        minuteInput->setEditMode(gui::EditMode::Edit);

        separatorLabel = new gui::Label(
            hBox, 0, 0, style::settings::widget::time::separator_w, style::settings::widget::time::body_h);
        separatorLabel->setEdges(gui::RectangleEdge::None);
        separatorLabel->activeItem = false;

        meridiemInput =
            new gui::Label(hBox, 0, 0, style::settings::widget::time::input_w, style::settings::widget::time::body_h);
        meridiemInput->setEdges(gui::RectangleEdge::Bottom);
        meridiemInput->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        meridiemInput->setFont(style::window::font::largelight);
        meridiemInput->setPenFocusWidth(style::window::default_border_focus_w);
        meridiemInput->setPenWidth(style::window::default_border_rect_no_focus);

        applyFocusCallbacks();
        applyInputCallbacks();
        setTime(std::chrono::minutes(style::settings::widget::time::default_time_in_minutes));
    }

    auto TimeWidget::getTime() const noexcept -> std::chrono::minutes
    {
        std::chrono::minutes currentTimeDuration;
        int currentHourRaw;
        int currentMinutesRaw;

        try {
            currentHourRaw = std::stoi(hourInput->getText());
        }
        catch (const std::exception &e) {
            LOG_ERROR("TimeWidget::getTime::currentHourRaw %s", e.what());
            currentHourRaw = 0;
        }
        if (meridiemInput->getText() == style::settings::widget::time::after_noon) {
            currentHourRaw += style::settings::widget::time::max_hour;
        }
        try {
            currentMinutesRaw = std::stoi(minuteInput->getText());
        }
        catch (const std::exception &e) {
            LOG_ERROR("TimeWidget::getTime::currentMinutesRaw %s", e.what());
            currentMinutesRaw = 0;
        }
        currentTimeDuration = std::chrono::hours(currentHourRaw);
        currentTimeDuration += std::chrono::minutes(currentMinutesRaw);

        return currentTimeDuration;
    }

    void TimeWidget::setTime(std::chrono::minutes newTimeMinutes)
    {
        auto isPm = false;

        auto newTimeHours = std::chrono::duration_cast<std::chrono::hours>(newTimeMinutes);
        newTimeMinutes -= std::chrono::duration_cast<std::chrono::minutes>(newTimeHours);

        auto newHourRaw   = newTimeHours.count();
        auto newMinuteRaw = newTimeMinutes.count();
        if (newHourRaw > style::settings::widget::time::max_hour) {
            newHourRaw -= style::settings::widget::time::max_hour;
            isPm = true;
        }

        hourInput->setText(std::to_string(newHourRaw));
        minuteInput->setText(std::to_string(newMinuteRaw));
        if (isPm) {
            meridiemInput->setText(style::settings::widget::time::after_noon);
        }
        else {
            meridiemInput->setText(style::settings::widget::time::before_noon);
        }
    }

    void TimeWidget::applyFocusCallbacks()
    {
        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? hBox : nullptr);
            return true;
        };

        hourInput->focusChangedCallback = [&](Item &item) {
            if (!item.focus) {
                if (hourInput->isEmpty()) {
                    hourInput->setText(style::settings::widget::time::default_input_string);
                }
            }
            return true;
        };

        minuteInput->focusChangedCallback = [&](Item &item) {
            if (!item.focus) {
                if (minuteInput->isEmpty()) {
                    minuteInput->setText(style::settings::widget::time::default_input_string);
                }
            }
            return true;
        };

        meridiemInput->focusChangedCallback = [&](Item &item) {
            if (item.focus) {
                bottomBarTemporaryMode(utils::localize.get("common_switch"));
            }
            else {
                bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };
    }

    void TimeWidget::applyInputCallbacks()
    {
        inputCallback = [&](Item &item, const InputEvent &event) {
            auto focusedItem = getFocusItem();
            if (!event.isShortPress()) {
                return false;
            }
            if (event.is(gui::KeyCode::KEY_ENTER) || event.is(gui::KeyCode::KEY_RF)) {
                return false;
            }

            if (focusedItem->onInput(event)) {
                return readTimeFromInputFields();
            }
            else if (hBox->onInput(event)) {
                return true;
            }

            return false;
        };

        auto isMoveDeleteSaveEvent = [](auto inputEvent) {
            return inputEvent.is(gui::KeyCode::KEY_LEFT) || inputEvent.is(gui::KeyCode::KEY_RIGHT) ||
                   inputEvent.is(gui::KeyCode::KEY_PND) || inputEvent.is(gui::KeyCode::KEY_UP) ||
                   inputEvent.is(gui::KeyCode::KEY_DOWN) || inputEvent.is(gui::KeyCode::KEY_RF) ||
                   inputEvent.is(gui::KeyCode::KEY_ENTER);
        };

        hourInput->inputCallback = [&](Item &item, const InputEvent &event) {
            if (!event.isShortPress()) {
                return false;
            }
            if (hourInput->getText().length() > 1 && !isMoveDeleteSaveEvent(event)) {
                return true;
            }
            return false;
        };

        minuteInput->inputCallback = [&](Item &item, const InputEvent &event) {
            if (!event.isShortPress()) {
                return false;
            }
            if (minuteInput->getText().length() > 1 && !isMoveDeleteSaveEvent(event)) {
                return true;
            }
            return false;
        };

        meridiemInput->inputCallback = [&](Item &item, const InputEvent &event) {
            if (!event.isShortPress()) {
                return false;
            }
            if (event.is(gui::KeyCode::KEY_LF)) {
                if (meridiemInput->getText() == style::settings::widget::time::before_noon) {
                    meridiemInput->setText(style::settings::widget::time::after_noon);
                }
                else {
                    meridiemInput->setText(style::settings::widget::time::before_noon);
                }
                return true;
            }
            return false;
        };
    }

    auto TimeWidget::readTimeFromInputFields() const -> bool
    {
        uint32_t hours;
        uint32_t minutes;

        try {
            hours = std::stoi(hourInput->getText().c_str());
        }
        catch (const std::exception &e) {
            LOG_ERROR("EventTimeItem::applyInputCallbacks hours: %s", e.what());
            return false;
        }

        try {
            minutes = std::stoi(minuteInput->getText().c_str());
        }
        catch (const std::exception &e) {
            LOG_ERROR("EventTimeItem::applyInputCallbacks minutes: %s", e.what());
            return false;
        }

        if (hours > style::settings::widget::time::max_hour) {
            hourInput->setText(style::settings::widget::time::max_hour_string);
        }
        if (minutes > style::settings::widget::time::max_minutes) {
            minuteInput->setText(style::settings::widget::time::max_minutes_string);
        }

        return true;
    }
} /* namespace gui */
