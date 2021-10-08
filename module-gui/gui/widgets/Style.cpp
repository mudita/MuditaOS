// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Style.hpp"
#include <Label.hpp>

namespace style::window
{

    void decorate(gui::Rect *el)
    {
        if (el == nullptr) {
            return;
        }
        el->setPenWidth(default_border_no_focus_w);
        el->setPenFocusWidth(default_border_focus_w);
        el->setEdges(gui::RectangleEdge::Top | gui::RectangleEdge::Bottom);
        el->setFilled(false);
    }

    void decorate(gui::Label *el)
    {
        if (el == nullptr) {
            return;
        }
        decorate(static_cast<gui::Rect *>(el));
        el->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
    }

    void decorateOption(gui::Label *el)
    {
        if (el == nullptr) {
            return;
        }
        decorate(el);
        el->setMargins(gui::Margins(style::window::default_left_margin, 0, 0, 0));
        el->setPadding(gui::Padding(style::padding::default_left_text_padding, 0, 0, 0));
        el->setFont(style::window::font::big);
        el->setMinimumHeight(style::window::label::big_h);
        el->setMaximumWidth(style::window::default_body_width);
    }

}; // namespace style::window
