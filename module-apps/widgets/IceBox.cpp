// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IceBox.hpp"

#include <Text.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    IceBox::IceBox(Item *parent)
        : HBox(parent, style::window::ice::x, style::window::ice::y, style::window::ice::w, style::window::ice::h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        auto arrow        = new gui::Image("left_label_arrow");
        arrow->activeItem = false;
        arrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        arrow->setMargins(Margins(0, 0, style::window::ice::margin, 0));
        addWidget(arrow);

        auto iceText        = new gui::Text(nullptr, 0, 0, style::window::ice::text::w, style::window::ice::h);
        iceText->activeItem = false;
        iceText->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        iceText->setFont(style::window::font::verysmall);
        iceText->setText(utils::translate("app_desktop_emergency"));
        addWidget(iceText);
    }
} // namespace gui
