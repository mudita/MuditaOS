// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IceAction.hpp"
#include "Style.hpp"

#include <Text.hpp>
#include <Image.hpp>
#include <i18n/i18n.hpp>

namespace gui::header
{
    IceAction::IceAction(Item *parent)
        : HBox(parent, 0, 0, style::header::navigation_indicator::w, style::header::navigation_indicator::h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);
        setMargins(Margins(style::header::navigation_indicator::right_left_margin,
                           style::header::navigation_indicator::top_margin,
                           0,
                           0));

        auto arrow        = new gui::Image("arrow_left_24px_W_G");
        arrow->activeItem = false;
        arrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        addWidget(arrow);

        auto iceText = new gui::Text(nullptr, 0, 0, 0, 0);
        iceText->setMaximumSize(style::header::navigation_indicator::ice_w, style::header::navigation_indicator::h);
        iceText->activeItem = false;
        iceText->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        iceText->setFont(style::window::font::verysmall);
        iceText->setText(utils::translate("app_desktop_emergency"));
        addWidget(iceText);
    }
} // namespace gui::header
