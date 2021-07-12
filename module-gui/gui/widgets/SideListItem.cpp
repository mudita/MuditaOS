// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Label.hpp"
#include "ListItemWithDescription.hpp"
#include "SideListItem.hpp"
#include "Style.hpp"

namespace gui
{
    SideListItem::SideListItem(std::string description) : ListItemWithDescription(description)
    {
        body = new HBox(this, 0U, 0U, style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        body->setEdges(RectangleEdge::None);

        auto topMessage =
            new Label(body, 0U, 0U, style::sidelistview::top_message::w, style::sidelistview::top_message::h);
        topMessage->setFont(style::sidelistview::top_message::font);
        topMessage->setEdges(gui::RectangleEdge::None);
        topMessage->activeItem = false;
        topMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        topMessage->setText(description);
    }
} /* namespace gui */
