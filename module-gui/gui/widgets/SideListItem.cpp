// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SideListItem.hpp"
#include "Style.hpp"

namespace gui
{

    SideListItem::SideListItem(std::string description) : description{description}
    {
        setRadius(0);
        setEdges(RectangleEdge::None);

        setPenFocusWidth(style::window::default_border_focus_w);
        setPenWidth(style::window::default_border_no_focus_w);

        type = ItemType::LIST_ITEM;
    }

    auto SideListItem::getDescription() const noexcept -> std::string
    {
        return this->description;
    }

    auto SideListItem::setDescription(std::string description) -> void
    {
        this->description = std::move(description);
    }

} /* namespace gui */
