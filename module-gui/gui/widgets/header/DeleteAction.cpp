// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeleteAction.hpp"
#include "Style.hpp"

#include <Text.hpp>
#include <Image.hpp>
#include <i18n/i18n.hpp>

namespace gui::header
{
    DeleteAction::DeleteAction(Item *parent)
        : HBox(parent, 0, 0, style::header::navigation_indicator::w, style::header::navigation_indicator::h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);
        setMargins(Margins(0,
                           style::header::navigation_indicator::top_margin,
                           style::header::navigation_indicator::right_left_margin,
                           0));

        auto wasteBin        = new gui::Image("delete_24px_W_G");
        wasteBin->activeItem = false;
        wasteBin->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        addWidget(wasteBin);

        auto arrow        = new gui::Image("arrow_right_24px_W_G");
        arrow->activeItem = false;
        arrow->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        addWidget(arrow);
    }
} // namespace gui::header
