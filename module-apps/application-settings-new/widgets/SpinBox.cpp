// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SpinBox.hpp"
#include "OptionStyle.hpp"
#include "widgets/BarGraph.hpp"

#include <InputEvent.hpp>
#include <Label.hpp>
#include <Image.hpp>
#include <utf8/UTF8.hpp>
#include <utility>

namespace gui
{
    SpinBox::SpinBox(
        Item *parent, const std::string &title, UpdateCallback updateCallback, uint8_t maxValue, uint8_t startValue)
        : HBox(parent, style::window::default_left_margin), updateBarCallback(std::move(updateCallback))
    {
        setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        setPenWidth(style::window::default_border_no_focus_w);
        setPenFocusWidth(style::window::default_border_focus_w);
        setEdges(gui::RectangleEdge::Top | gui::RectangleEdge::Bottom);

        titleLabel = addTitle(this, title);
        leftArrow  = addArrow(this, "arrow_left", Alignment::Horizontal::Left, false);
        rightArrow = addArrow(this, "arrow_right", Alignment::Horizontal::Right, false);
        bar        = addBarGraph(this, maxValue, startValue);

        focusChangedCallback = [this](Item &item) {
            leftArrow->setVisible(item.focus);
            rightArrow->setVisible(item.focus);
            resizeItems();
            return true;
        };

        inputCallback = [this](gui::Item &item, const gui::InputEvent &event) {
            if (!event.isShortPress()) {
                return false;
            }

            int update = 0;
            if (event.is(KeyCode::KEY_LEFT)) {
                update = -1;
            }
            else if (event.is(KeyCode::KEY_RIGHT)) {
                update = 1;
            }

            if (update != 0 && bar->update(update)) {
                updateBarCallback(bar->getValue());
                return true;
            }

            return false;
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

    Label *SpinBox::addTitle(Item *parent, const std::string &text)
    {
        auto label = new Label(parent);
        label->setMinimumHeight(style::window::label::default_h);
        label->setMaximumWidth(style::window::default_body_width);
        label->setMargins(Margins(option::window::option_left_margin, 0, 0, 0));
        label->setEdges(RectangleEdge::None);
        label->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        label->setFont(style::window::font::big);
        label->setText(text);
        label->activeItem = false;

        return label;
    }

    HBarGraph *SpinBox::addBarGraph(Item *parent, uint8_t maxValue, uint8_t startValue)
    {
        auto barGraph = new HBarGraph(parent, 0, 0, maxValue);
        barGraph->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        barGraph->setMargins(Margins(0, 0, option::window::option_right_margin, 0));
        barGraph->setValue(startValue);
        barGraph->activeItem = false;

        return barGraph;
    }
} // namespace gui
