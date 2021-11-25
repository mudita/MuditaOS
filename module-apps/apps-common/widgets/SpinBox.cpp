// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SpinBox.hpp"

#include <OptionStyle.hpp>
#include <widgets/BarGraph.hpp>
#include <InputEvent.hpp>
#include <Text.hpp>
#include <Image.hpp>
#include <utility>

namespace gui
{
    SpinBox::SpinBox(Item *parent,
                     const std::string &title,
                     UpdateCallback updateCallback,
                     uint8_t maxValue,
                     uint8_t startValue,
                     std::function<void(const UTF8 &text)> navBarTemporaryMode,
                     std::function<void()> navBarRestoreFromTemporaryMode)
        : HBox(parent, style::window::default_left_margin), updateBarCallback(std::move(updateCallback)),
          navBarTemporaryMode(std::move(navBarTemporaryMode)),
          navBarRestoreFromTemporaryMode(std::move(navBarRestoreFromTemporaryMode))
    {
        setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        setPenWidth(style::window::default_border_no_focus_w);
        setPenFocusWidth(style::window::default_border_focus_w);
        setEdges(gui::RectangleEdge::Top | gui::RectangleEdge::Bottom);

        titleLabel = addTitle(this, title);
        leftArrow  = addArrow(this, "arrow_left_20px_W_M", Alignment::Horizontal::Left, false);
        rightArrow = addArrow(this, "arrow_right_20px_W_M", Alignment::Horizontal::Right, false);
        bar        = addBarGraph(this, maxValue, startValue);

        focusChangedCallback = [this](Item &item) {
            leftArrow->setVisible(item.focus);
            rightArrow->setVisible(item.focus);
            resizeItems();

            if (item.focus) {
                if (this->navBarTemporaryMode) {
                    this->navBarTemporaryMode("");
                }
            }
            else {
                if (this->navBarRestoreFromTemporaryMode) {
                    this->navBarRestoreFromTemporaryMode();
                }
            }
            return true;
        };

        inputCallback = [this](gui::Item &item, const gui::InputEvent &event) {
            auto ret = false;
            if (!event.isShortRelease()) {
                return false;
            }

            int update = bar->getValue();
            if (event.is(KeyCode::KEY_LEFT)) {
                update--;
                ret = true;
            }
            else if (event.is(KeyCode::KEY_RIGHT)) {
                update++;
                ret = true;
            }

            if (update >= 0 && bar->setValue(update)) {
                updateBarCallback(bar->getValue());
            }

            return ret;
        };
    }

    Image *SpinBox::addArrow(Item *parent, const std::string &arrowName, Alignment::Horizontal aligment, bool visible)
    {
        auto arrow = new Image(parent, 0, 0, 0, 0);
        arrow->setAlignment(Alignment(aligment, Alignment::Vertical::Center));
        arrow->setMargins(Margins(0, 0, style::margins::big, 0));
        arrow->set(arrowName);
        arrow->setVisible(visible);

        return arrow;
    }

    Text *SpinBox::addTitle(Item *parent, const std::string &text)
    {
        auto title = new gui::Text(parent, 0, 0, 0, 0);
        title->setFont(style::window::font::big);
        title->setMinimumHeight(style::window::label::default_h);
        title->setMinimumWidthToFitText(text);
        title->setMargins(Margins(option::window::option_left_margin, 0, option::window::option_right_margin, 0));
        title->setEdges(RectangleEdge::None);
        title->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        title->setText(text);
        title->activeItem = false;

        return title;
    }

    HBarGraph *SpinBox::addBarGraph(Item *parent, uint8_t maxValue, uint8_t startValue)
    {
        auto barGraph = new HBarGraph(parent, 0, 0, maxValue);
        barGraph->setMaximumWidth(style::window::default_body_width);
        barGraph->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        barGraph->setMargins(Margins(0, 0, option::window::option_right_margin, 0));
        barGraph->setValue(startValue);
        barGraph->activeItem = false;

        return barGraph;
    }
} // namespace gui
