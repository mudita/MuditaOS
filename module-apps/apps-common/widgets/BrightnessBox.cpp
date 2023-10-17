// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <i18n/i18n.hpp>
#include "BightnessBox.hpp"

namespace gui
{
    BrightnessBox::BrightnessBox(Item *parent, uint32_t x, uint32_t y)
        : HBox(parent, x, y, style::window::brightness::title::width, style::window::brightness::title::height)
    {
        setEdges(RectangleEdge::None);
        addArrow("arrow_left_20px_W_M", Alignment::Horizontal::Left);
        addBrightnessTitle(this, utils::translate(style::window::brightness::title_key));
        addArrow("arrow_right_20px_W_M", Alignment::Horizontal::Right);

        resizeItems();
    }

    void BrightnessBox::addArrow(const std::string &arrowName, Alignment::Horizontal aligment)
    {
        auto arrow = new Image(this, 0, 0, 0, 0);
        arrow->setMinimumSize(style::window::brightness::title::arrow_width, style::window::brightness::title::height);
        arrow->setAlignment(Alignment(aligment, Alignment::Vertical::Center));
        arrow->set(arrowName);
    }

    void BrightnessBox::addBrightnessTitle(Item *parent, const std::string &text)
    {
        auto brightnessTitle = new Label(parent, 0, 0, 0, 0);
        brightnessTitle->setMinimumSize(style::window::brightness::title::label_width,
                                        style::window::brightness::title::label_height);

        brightnessTitle->setEdges(gui::RectangleEdge::None);
        brightnessTitle->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        brightnessTitle->activeItem = false;
        brightnessTitle->setText(text);
        brightnessTitle->setFont(style::window::font::medium);
        brightnessTitle->setMargins(Margins(
            style::window::brightness::title::space_width, 0, style::window::brightness::title::space_width, 0));
    }
} // namespace gui
