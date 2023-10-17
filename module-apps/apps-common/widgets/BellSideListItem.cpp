// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextFixedSize.hpp"
#include "BellSideListItem.hpp"
#include "Style.hpp"

namespace gui
{

    BellSideListItem::BellSideListItem()
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        body = new BellBaseLayout(this);
    }

    void BellSideListItem::setBottomDescriptionText(const std::string &description)
    {
        if (bottomText != nullptr) {
            bottomText->setText(description);
        }
    }

    void BellSideListItem::setupTopTextBox(const std::string &description)
    {
        auto topText = new TextFixedSize(body->firstBox);
        setupTextBox(topText, style::bell_sidelist_item::title_font, description);
    }

    void BellSideListItem::setupBottomTextBox(const std::string &description)
    {
        bottomText = new TextFixedSize(body->lastBox);
        setupTextBox(bottomText, style::bell_sidelist_item::description_font, description);
    }

    void BellSideListItem::setupTextBox(TextFixedSize *textBox,
                                        const std::string &fontName,
                                        const std::string &description)
    {
        textBox->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        textBox->setFont(fontName);
        textBox->setEdges(gui::RectangleEdge::None);
        textBox->activeItem = false;
        textBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        textBox->setRichText(description);
        textBox->drawUnderline(false);
    }

} /* namespace gui */
