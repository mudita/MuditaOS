// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmSetSpinner.hpp"
#include <FontManager.hpp>
#include <RawFont.hpp>
#include <gui/widgets/Label.hpp>

namespace gui
{
    AlarmSetSpinner::AlarmSetSpinner(Item *parent, Length x, Length y, Length w, Length h) : HBox(parent, x, y, w, h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        leftArrow = new ImageBox(this, 0, 0, 0, 0, new Image("bell_arrow_left"));
        leftArrow->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        leftArrow->setMargins(Margins(0, 0, 0, 0));
        leftArrow->setMinimumSize(style::alarm_set_spinner::arrow::w, style::alarm_set_spinner::arrow::h);

        alarmImg = new ImageBox(this, 0, 0, 0, 0, new Image("bell_alarm_deactivated"));
        alarmImg->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        alarmImg->setMargins(Margins(0, 0, 0, 0));
        alarmImg->setMinimumSize(style::alarm_set_spinner::arrow::w, style::alarm_set_spinner::arrow::h);

        timeSpinner = new TimeSetSpinner(this, 0, 0, 0, 0);
        timeSpinner->setFont(style::window::font::largelight);
        timeSpinner->setEditMode(EditMode::Scroll);
        timeSpinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        timeSpinner->setMargins(Margins(0, 0, 0, 0));
        timeSpinner->setMinimumHeight(style::alarm_set_spinner::h);

        rightArrow = new ImageBox(this, 0, 0, 0, 0, new Image("bell_arrow_right"));
        rightArrow->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        rightArrow->setMargins(Margins(0, 0, 0, 0));
        rightArrow->setMinimumSize(style::alarm_set_spinner::arrow::w, style::alarm_set_spinner::arrow::h);

        resizeItems();

        if (editMode == EditMode::Edit) {
            setFocusItem(timeSpinner);
        }
    }

    auto AlarmSetSpinner::onInput(const InputEvent &inputEvent) -> bool
    {
        LOG_INFO("Bell AlarmSetSpinner Input");
        return this->focusItem->onInput(inputEvent);
    }

    auto AlarmSetSpinner::setHour(int value) noexcept -> void
    {
        timeSpinner->setHour(value);
    }

    auto AlarmSetSpinner::setMinute(int value) noexcept -> void
    {
        timeSpinner->setMinute(value);
    }

    auto AlarmSetSpinner::setFont(std::string newFontName) noexcept -> void
    {
        fontName = std::move(newFontName);
        timeSpinner->setFont(fontName);
    }

    auto AlarmSetSpinner::setEditMode(EditMode mode) noexcept -> void
    {
        editMode = mode;
        if (editMode == EditMode::Edit) {
            setFocusItem(timeSpinner);
            leftArrow->setVisible(true);
            rightArrow->setVisible(true);
        }
        else {
            setFocusItem(nullptr);
            leftArrow->setVisible(false);
            rightArrow->setVisible(false);
        }

        timeSpinner->setEditMode(editMode);
    }

    auto AlarmSetSpinner::getHour() const noexcept -> int
    {
        return timeSpinner->getHour();
    }

    auto AlarmSetSpinner::getMinute() const noexcept -> int
    {
        return timeSpinner->getMinute();
    }

    auto AlarmSetSpinner::setAlarmStatus(BellAlarm::Status status) noexcept -> void
    {
        alarmStatus = status;

        switch (alarmStatus) {
        case BellAlarm::Status::ACTIVATED:
            alarmImg->setImage("bell_alarm_activated");
            break;
        case BellAlarm::Status::DEACTIVATED:
            alarmImg->setImage("bell_alarm_deactivated");
            break;
        case BellAlarm::Status::RINGING:
            alarmImg->setImage("bell_alarm_ringing");
            break;
        case BellAlarm::Status::SNOOZE:
            alarmImg->setImage("bell_alarm_snooze");
            break;
        case BellAlarm::Status::UNKNOWN:
            alarmImg->setImage("bell_alarm_deactivated");
            break;
        default:
            alarmImg->setImage("bell_alarm_deactivated");
            break;
        }
    }

    auto AlarmSetSpinner::getAlarmStatus() noexcept -> BellAlarm::Status
    {
        return alarmStatus;
    }

} /* namespace gui */
