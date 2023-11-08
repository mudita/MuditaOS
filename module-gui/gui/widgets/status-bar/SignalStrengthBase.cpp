// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SignalStrengthBase.hpp"

namespace gui::status_bar
{
    SignalStrengthBase::SignalStrengthBase(
        Item *parent, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h)
        : StatusBarWidgetBase(parent, x, y, w, h)
    {
        setEdges(RectangleEdge::None);
        setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom));
    }
} // namespace gui::status_bar
