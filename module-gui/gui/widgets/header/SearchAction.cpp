// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SearchAction.hpp"
#include "Style.hpp"

#include <Text.hpp>
#include <Image.hpp>

namespace gui::header
{
    SearchAction::SearchAction(Item *parent)
        : HBox(parent, 0, 0, style::header::navigation_indicator::w, style::header::navigation_indicator::h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);
        setMargins(Margins(0,
                           style::header::navigation_indicator::top_margin,
                           style::header::navigation_indicator::right_left_margin,
                           0));
        setReverseOrder(true);

        auto arrow = new gui::Image("arrow_right_24px_W_G");
        arrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        addWidget(arrow);

        auto search = new gui::Image("search_32px_W_G");
        search->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        addWidget(search);
    }
} // namespace gui::header
