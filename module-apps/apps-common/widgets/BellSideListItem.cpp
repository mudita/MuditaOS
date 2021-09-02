// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Label.hpp"
#include "ListItemWithDescription.hpp"
#include "BellSideListItem.hpp"
#include "Style.hpp"

namespace gui
{
    BellSideListItem::BellSideListItem(const std::string &description) : ListItemWithDescription(description)
    {
        setEdges(RectangleEdge::None);
        body = new BellBaseLayout(this);

        auto topMessage = new Label(body->firstBox);
        topMessage->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        topMessage->setFont(style::bell_sidelist_item::description_font);
        topMessage->setEdges(gui::RectangleEdge::None);
        topMessage->activeItem = false;
        topMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        topMessage->setText(description);
    }
} /* namespace gui */
