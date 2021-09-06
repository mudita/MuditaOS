// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmItem.hpp"
#include "AlarmClockStyle.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include <InputEvent.hpp>
#include <time/dateCommon.hpp>
#include <time/time_conversion.hpp>

namespace gui
{
    AlarmItem::AlarmItem(std::shared_ptr<app::alarmClock::AlarmRRulePresenter> presenter)
        : AlarmRRuleItem(std::move(presenter))
    {
        setMinimumSize(style::window::default_body_width, style::alarmClock::window::item::height);
        setMargins(gui::Margins(0, style::margins::small, 0, style::alarmClock::window::item::botMargin));

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);

        vBox = new gui::VBox(hBox, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdge::None);
        vBox->setMaximumSize(style::window::default_body_width, style::alarmClock::window::item::height);
        vBox->setMargins(gui::Margins(style::widgets::leftMargin, 0, 0, 0));

        timeLabel = new gui::Label(vBox, 0, 0, 0, 0);
        timeLabel->setEdges(gui::RectangleEdge::None);
        timeLabel->setMaximumSize(style::window::default_body_width, style::alarmClock::window::item::timeHeight);
        timeLabel->setMargins(gui::Margins(0, style::margins::small, 0, 0));
        timeLabel->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        timeLabel->setFont(style::window::font::largelight);

        periodLabel = new gui::Label(vBox, 0, 0, 0, 0);
        periodLabel->setEdges(gui::RectangleEdge::None);
        periodLabel->setMaximumSize(style::window::default_body_width, style::alarmClock::window::item::periodHeight);
        periodLabel->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top});
        periodLabel->setFont(style::window::font::small);

        onOffImage = new gui::ButtonOnOff(hBox, ButtonState::On);
        onOffImage->setMargins(gui::Margins(0, 0, style::widgets::rightMargin, 0));

        setAlarm();

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            hBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void AlarmItem::setAlarm()
    {
        timeLabel->setText(TimePointToLocalizedTimeString(getPresenter()->getAlarm()->startDate));
        onOffImage->switchState(getPresenter()->getAlarm()->enabled ? ButtonState::Off : ButtonState::On);

        if (getPresenter()->hasRecurrence()) {
            periodLabel->setText(getPresenter()->getDescription());
        }

        if (periodLabel->getText().empty()) {
            periodLabel->setMaximumSize(0, 0);
            timeLabel->setMaximumSize(style::window::default_body_width,
                                      style::alarmClock::window::item::timeHeight +
                                          style::alarmClock::window::item::periodHeight);
            vBox->resizeItems();
        }
    }
} // namespace gui
