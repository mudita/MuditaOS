// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalBox.hpp"

#include "application-meditation/data/Style.hpp"
#include "InputEvent.hpp"

#include <module-utils/i18n/i18n.hpp>
#include <cassert>

using namespace gui;

namespace
{
    using minutes = std::chrono::minutes;
    const static std::vector<minutes> chimeIntervals{
        minutes{0}, minutes{2}, minutes{5}, minutes{10}, minutes{15}, minutes{30}};
} // namespace

IntervalBox::IntervalBox(Item *parent, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h)
    : BoxLayout(parent, x, y, w, h)
{
    build();
}

void IntervalBox::build()
{
    namespace boxStyle = style::meditation::intervalBox;

    Label *topLabel = new Label(this,
                                boxStyle::topLabel::X,
                                boxStyle::topLabel::Y,
                                boxStyle::topLabel::Width,
                                boxStyle::topLabel::Height,
                                utils::localize.get("app_meditation_interval_chime"));
    topLabel->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom));
    topLabel->setFont(style::window::font::verysmall);
    topLabel->setEdges(RectangleEdge::None);

    bottomLabel = new Label(this,
                            boxStyle::bottomLabel::X,
                            boxStyle::bottomLabel::Y,
                            boxStyle::bottomLabel::Width,
                            boxStyle::bottomLabel::Height);
    bottomLabel->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
    bottomLabel->setFont(style::window::font::small);
    bottomLabel->setEdges(RectangleEdge::Bottom);

    leftSwitchArrow  = new gui::Image(bottomLabel,
                                     boxStyle::arrow::LeftX,
                                     boxStyle::arrow::Y,
                                     boxStyle::arrow::Width,
                                     boxStyle::arrow::Height,
                                     "left_label_arrow");
    rightSwitchArrow = new gui::Image(bottomLabel,
                                      boxStyle::arrow::RightX,
                                      boxStyle::arrow::Y,
                                      boxStyle::arrow::Width,
                                      boxStyle::arrow::Height,
                                      "right_label_arrow");

    leftSwitchArrow->setVisible(false);
    rightSwitchArrow->setVisible(false);

    updateIntervals(ChimeIntervalList::Direction::Next);
}

bool IntervalBox::onFocus(bool state)
{
    if (state) {
        bottomLabel->setFont(style::window::font::smallbold);
    }
    else {
        bottomLabel->setFont(style::window::font::small);
    }
    leftSwitchArrow->setVisible(state && showLeftArrowOnFocus);
    rightSwitchArrow->setVisible(state && showRightArrowOnFocus);
    return true;
}

bool IntervalBox::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortPress()) {
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
    if (!chimeIntervals.moveToNext(direction)) {
        return;
    }
    intervalValue = chimeIntervals.getCurrent();
    bottomLabel->setText(ChimeIntervalList::toPrintableInterval(intervalValue));

    showLeftArrowOnFocus  = chimeIntervals.hasNext(ChimeIntervalList::Direction::Previous);
    showRightArrowOnFocus = chimeIntervals.hasNext(ChimeIntervalList::Direction::Next);

    leftSwitchArrow->setVisible(showLeftArrowOnFocus);
    rightSwitchArrow->setVisible(showRightArrowOnFocus);
}

IntervalBox::ChimeIntervalList::ChimeIntervalList() : intervals(::chimeIntervals), current(intervals.begin())
{}

bool IntervalBox::ChimeIntervalList::moveToNext(Direction direction) noexcept
{
    if (!hasNext(direction)) {
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

bool IntervalBox::ChimeIntervalList::hasNext(Direction direction) const noexcept
{
    if (direction == Direction::Next) {
        return std::next(current) != intervals.end();
    }
    return current != intervals.begin();
}

std::string IntervalBox::ChimeIntervalList::toPrintableInterval(std::chrono::minutes value)
{
    if (value.count() == 0) {
        return utils::localize.get("app_meditation_interval_none");
    }
    const std::string toReplace = "%0";
    std::string temp            = utils::localize.get("app_meditation_interval_every_x_minutes");
    temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(static_cast<int>(value.count())));
    return temp;
}
