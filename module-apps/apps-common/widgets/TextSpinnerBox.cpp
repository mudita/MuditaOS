// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextSpinnerBox.hpp"

namespace gui
{
    TextSpinnerBox::TextSpinnerBox(Item *parent, const std::vector<UTF8> &data, Boundaries boundaries) : HBox(parent)
    {
        setEdges(RectangleEdge::Bottom);

        leftArrow = new gui::ImageBox(this, new Image("arrow_left_24px_W_G"));
        leftArrow->setMinimumSizeToFitImage();
        leftArrow->setVisible(false);

        spinner = new UTF8Spinner(data, Boundaries::Continuous, Orientation::Horizontal);
        spinner->setFont(style::window::font::medium);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setEdges(RectangleEdge::All);
        spinner->setVisible(true);
        addWidget(spinner);

        rightArrow = new gui::ImageBox(this, new Image("arrow_right_24px_W_G"));
        rightArrow->setMinimumSizeToFitImage();
        rightArrow->setVisible(false);

        inputCallback = [&](Item &item, const InputEvent &event) { return spinner->onInput(event); };

        focusChangedCallback = [&](gui::Item &item) {
            if (focus) {
                leftArrow->setVisible(true);
                rightArrow->setVisible(true);
                spinner->setFont(style::window::font::mediumbold);
                setPenWidth(style::window::default_border_focus_w);
            }
            else {
                leftArrow->setVisible(false);
                rightArrow->setVisible(false);
                spinner->setFont(style::window::font::medium);
                setPenWidth(style::window::default_border_rect_no_focus);
            }
            resizeItems();
            return true;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            spinner->setMaximumSize(widgetArea.w, widgetArea.h);
            resizeItems();
            return true;
        };
    }

    void TextSpinnerBox::setData(const std::vector<UTF8> &data)
    {
        spinner->setRange(data);
    }

    UTF8 TextSpinnerBox::getCurrentValue() const noexcept
    {
        return spinner->getCurrentValue();
    }

    void TextSpinnerBox::setCurrentValue(UTF8 val)
    {
        spinner->setCurrentValue(val);
    }
} // namespace gui
