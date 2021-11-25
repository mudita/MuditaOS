// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalBox.hpp"
#include "Style.hpp"
#include "TimerProperty.hpp"

#include <gui/input/InputEvent.hpp>
#include <i18n/i18n.hpp>

#include <cassert>

using namespace gui;

namespace
{
    using minutes = std::chrono::minutes;
    const std::vector<minutes> chimeIntervals{
        minutes{0}, minutes{2}, minutes{5}, minutes{10}, minutes{15}, minutes{30}};
} // namespace

IntervalBox::IntervalBox(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, TimerProperty *timerSetter)
    : BoxLayout(parent, x, y, w, h), timerSetter(timerSetter)
{
    assert(timerSetter);
    build();
}

void IntervalBox::build()
{
    namespace boxStyle = style::meditation::intervalBox;

    auto topLabel = new Label(this,
                              boxStyle::topLabel::X,
                              boxStyle::topLabel::Y,
                              boxStyle::topLabel::Width,
                              boxStyle::topLabel::Height,
                              utils::translate("app_meditation_interval_chime"));
    topLabel->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom));
    topLabel->setFont(style::window::font::small);
    topLabel->setEdges(RectangleEdge::None);

    bottomLabel = new Label(this,
                            boxStyle::bottomLabel::X,
                            boxStyle::bottomLabel::Y,
                            boxStyle::bottomLabel::Width,
                            boxStyle::bottomLabel::Height);
    bottomLabel->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
    bottomLabel->setFont(style::window::font::small);
    bottomLabel->setEdges(RectangleEdge::Bottom);
    bottomLabel->setPenWidth(style::window::default_border_rect_no_focus);

    leftSwitchArrow  = new gui::Image(bottomLabel,
                                     boxStyle::arrow::LeftX,
                                     boxStyle::arrow::Y,
                                     boxStyle::arrow::Width,
                                     boxStyle::arrow::Height,
                                     "arrow_left_20px_W_M");
    rightSwitchArrow = new gui::Image(bottomLabel,
                                      boxStyle::arrow::RightX,
                                      boxStyle::arrow::Y,
                                      boxStyle::arrow::Width,
                                      boxStyle::arrow::Height,
                                      "arrow_right_20px_W_M");

    updateIntervals(ChimeIntervalList::Direction::Next);
    leftSwitchArrow->setVisible(false);
    rightSwitchArrow->setVisible(false);
}

bool IntervalBox::onFocus(bool state)
{
    if (state) {
        rescaleIntervals();
        bottomLabel->setPenWidth(style::window::default_border_focus_w);
        bottomLabel->setFont(style::window::font::smallbold);
    }
    else {
        bottomLabel->setPenWidth(style::window::default_border_rect_no_focus);
        bottomLabel->setFont(style::window::font::small);
    }
    auto currentMeditationTime = timerSetter->getTime();
    leftSwitchArrow->setVisible(state &&
                                chimeIntervals.hasNext(ChimeIntervalList::Direction::Previous, currentMeditationTime));
    rightSwitchArrow->setVisible(state &&
                                 chimeIntervals.hasNext(ChimeIntervalList::Direction::Next, currentMeditationTime));
    return true;
}

bool IntervalBox::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortRelease()) {
        if (inputEvent.is(KeyCode::KEY_LEFT)) {
            updateIntervals(ChimeIntervalList::Direction::Previous);
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_RIGHT)) {
            updateIntervals(ChimeIntervalList::Direction::Next);
            return true;
        }
    }
    return false;
}

void IntervalBox::updateIntervals(ChimeIntervalList::Direction direction)
{
    auto currentMeditationTime = timerSetter->getTime();
    if (!chimeIntervals.moveToNext(direction, currentMeditationTime)) {
        return;
    }
    intervalValue = chimeIntervals.getCurrent();
    bottomLabel->setText(ChimeIntervalList::toPrintableInterval(intervalValue));

    leftSwitchArrow->setVisible(chimeIntervals.hasNext(ChimeIntervalList::Direction::Previous, currentMeditationTime));
    rightSwitchArrow->setVisible(chimeIntervals.hasNext(ChimeIntervalList::Direction::Next, currentMeditationTime));
}

void IntervalBox::rescaleIntervals()
{
    while (intervalValue >= timerSetter->getTime() && intervalValue != minutes{0}) {
        updateIntervals(ChimeIntervalList::Direction::Previous);
    }
}

IntervalBox::ChimeIntervalList::ChimeIntervalList() : intervals(::chimeIntervals), current(intervals.begin())
{}

bool IntervalBox::ChimeIntervalList::moveToNext(Direction direction, std::chrono::minutes meditationTime) noexcept
{
    if (!hasNext(direction, meditationTime)) {
        return false;
    }
    if (direction == Direction::Next) {
        current++;
    }
    else {
        current--;
    }
    return true;
}

bool IntervalBox::ChimeIntervalList::hasNext(Direction direction, std::chrono::minutes meditationTime) const noexcept
{
    if (direction == Direction::Previous) {
        return current != intervals.begin();
    }
    auto result = std::next(current) != intervals.end();
    if (result) {
        result = *std::next(current) < meditationTime;
    }
    return result;
}

std::string IntervalBox::ChimeIntervalList::toPrintableInterval(std::chrono::minutes value)
{
    if (value.count() == 0) {
        return utils::translate("app_meditation_interval_none");
    }
    const std::string toReplace = "%0";
    std::string temp            = utils::translate("app_meditation_interval_every_x_minutes");
    temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(static_cast<int>(value.count())));
    return temp;
}
