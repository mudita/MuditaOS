// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellBaseLayout.hpp"
#include "Style.hpp"

namespace gui
{
    BellBaseLayout::BellBaseLayout(Item *parent, Position x, Position y, Length w, Length h)
        : VThreeBox(parent, x, y, w, h)
    {
        setMinimumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        firstBox = new VBox(this, 0, 0, 0, 0);
        firstBox->setMinimumSize(style::bell_base_layout::w, style::bell_base_layout::first_layout_min_h);
        firstBox->setMaximumHeight(style::bell_base_layout::outer_layouts_h);
        firstBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        firstBox->setEdges(RectangleEdge::None);
        firstBox->activeItem = false;

        centerBox = new VBox(this, 0, 0, 0, 0);
        centerBox->setEdges(RectangleEdge::None);
        centerBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        centerBox->setMinimumSize(style::bell_base_layout::w,
                                  style::bell_base_layout::h - 2 * style::bell_base_layout::outer_layouts_h);

        lastBox = new VBox(this, 0, 0, 0, 0);
        lastBox->setMinimumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        lastBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        lastBox->setEdges(RectangleEdge::None);
        lastBox->activeItem = false;

        resizeItems();
    }
} // namespace gui
