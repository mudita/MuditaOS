// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextFixedSize.hpp"
#include "ListItemWithDescription.hpp"
#include "BellSideListItem.hpp"
#include "Style.hpp"

namespace gui
{
    BellSideListItem::BellSideListItem(const std::string &description) : ListItemWithDescription(description)
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        body = new BellBaseLayout(this);

        auto topMessage = new TextFixedSize(body->firstBox);
        topMessage->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        topMessage->setFont(style::bell_sidelist_item::title_font);
        topMessage->setEdges(gui::RectangleEdge::None);
        topMessage->activeItem = false;
        topMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        topMessage->setRichText(description);
        topMessage->drawUnderline(false);
    }

    void BellSideListItem::setupBottomDescription(const std::string &description)
    {
        bottomText = new TextFixedSize(body->lastBox);
        bottomText->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::outer_layouts_h);
        bottomText->setFont(::style::bell_sidelist_item::description_font);
        bottomText->setEdges(RectangleEdge::None);
        bottomText->activeItem = false;
        bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        bottomText->setRichText(description);
        bottomText->drawUnderline(false);
    }

    void BellSideListItem::setBottomDescribtionText(const std::string &description)
    {
        if (bottomText != nullptr) {
            bottomText->setText(description);
        }
    }

} /* namespace gui */
