// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmItem.hpp"
#include "AlarmClockStyle.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include <InputEvent.hpp>
#include <module-utils/time/time_conversion.hpp>

namespace gui
{
    AlarmItem::AlarmItem(std::shared_ptr<AlarmsRecord> record) : alarm(std::move(record))
    {
        assert(alarm != nullptr);
        setMinimumSize(style::window::default_body_width, style::alarmClock::window::item::height);
        setMargins(gui::Margins(0, style::margins::small, 0, style::alarmClock::window::item::botMargin));

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);

        vBox = new gui::VBox(hBox, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdge::None);
        vBox->setMinimumSize(style::alarmClock::window::item::vBoxWidth, style::alarmClock::window::item::height);
        vBox->setMargins(gui::Margins(style::alarmClock::window::item::vBoxLeftMargin, 0, 0, 0));

        timeLabel = new gui::Label(vBox, 0, 0, 0, 0);
        timeLabel->setEdges(gui::RectangleEdge::None);
        timeLabel->setMinimumSize(style::alarmClock::window::item::vBoxWidth,
                                  style::alarmClock::window::item::timeHeight);
        timeLabel->setMargins(gui::Margins(0, style::margins::small, 0, 0));
        timeLabel->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        timeLabel->setFont(style::window::font::largelight);

        periodLabel = new gui::Label(vBox, 0, 0, 0, 0);
        periodLabel->setEdges(gui::RectangleEdge::None);
        periodLabel->setMinimumSize(style::alarmClock::window::item::vBoxWidth,
                                    style::alarmClock::window::item::periodHeight);
        periodLabel->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top});
        periodLabel->setFont(style::window::font::small);

        onOffImage = new gui::ButtonOnOff(hBox, ButtonState::On);
        onOffImage->setMargins(gui::Margins(style::alarmClock::window::item::imageMargin, 0, 0, 0));

        setAlarm();
    }

    bool AlarmItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);
        return true;
    }

    void AlarmItem::setAlarm()
    {
        timeLabel->setText(utils::time::CalendarConversion::TimePointToLocalizedTimeString(
            alarm->time, utils::time::Locale::format(utils::time::Locale::FormatTime12H)));
        if (alarm->status == AlarmStatus::Off) {
            onOffImage->switchState(ButtonState::Off);
        }
        if (alarm->repeat == static_cast<uint32_t>(AlarmRepeat::everyday)) {
            periodLabel->setText(utils::localize.get("app_alarm_clock_repeat_everyday"));
        }
        else if (alarm->repeat == static_cast<uint32_t>(AlarmRepeat::weekDays)) {
            periodLabel->setText(utils::localize.get("app_alarm_clock_repeat_week_days"));
        }
        else if (alarm->repeat != static_cast<uint32_t>(AlarmRepeat::never)) {
            periodLabel->setText(CustomRepeatValueParser(alarm->repeat).getWeekDaysText());
        }

        if (periodLabel->getText().empty()) {
            periodLabel->setMaximumSize(0, 0);
            timeLabel->setMinimumSize(style::alarmClock::window::item::vBoxWidth,
                                      style::alarmClock::window::item::timeHeight +
                                          style::alarmClock::window::item::periodHeight);
            vBox->resizeItems();
        }
    }
} // namespace gui
