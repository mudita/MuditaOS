// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AddElementAction.hpp"
#include "Style.hpp"

#include <Text.hpp>
#include <Image.hpp>

namespace gui::header
{
    AddElementAction::AddElementAction(Item *parent)
        : HBox(parent, 0, 0, style::header::navigation_indicator::w, style::header::navigation_indicator::h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);
        setMargins(Margins(style::header::navigation_indicator::right_left_margin,
                           style::header::navigation_indicator::top_margin,
                           0,
                           0));

        auto arrow = new gui::Image("arrow_left_24px_W_G");
        arrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        addWidget(arrow);

        auto plus = new gui::Image("plus_32px_W_G");
        plus->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        addWidget(plus);
    }
} // namespace gui::header
