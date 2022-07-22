// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SummaryListItem.hpp"

#include <Text.hpp>
#include <BoxLayout.hpp>

namespace
{
    constexpr auto width        = style::window::default_body_width;
    constexpr auto height       = 66;
    constexpr auto title_height = 33;
    constexpr auto value_height = 33;
} // namespace

namespace app::meditation
{
    using namespace gui;
    SummaryListItem::SummaryListItem(const std::string &titleText, const std::string &valueText)
    {
        setMinimumSize(width, height);
        setMargins(Margins(0, style::margins::big, 0, style::margins::huge));
        activeItem = false;

        body = new VBox(this, 0, 0, 0, 0);
        body->setEdges(RectangleEdge::None);

        title = new Text(body, 0, 0, 0, 0);
        title->setMinimumSize(width, title_height);
        title->setFont(style::window::font::bigbold);
        title->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        title->setText(titleText);

        value = new Text(body, 0, 0, 0, 0);
        value->setMinimumSize(width, value_height);
        value->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        value->setFont(style::window::font::big);
        value->setText(valueText);

        dimensionChangedCallback = [&]([[maybe_unused]] Item &item, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
} // namespace app::meditation
