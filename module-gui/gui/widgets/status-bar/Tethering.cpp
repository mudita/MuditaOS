// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Tethering.hpp"

#include <log/log.hpp>
#include <i18n/i18n.hpp>

#include "Item.hpp"
#include "Style.hpp"

namespace gui::status_bar
{
    Tethering::Tethering(Item *parent, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h)
        : StatusBarWidgetBase(parent, x, y, w, h)
    {
        setEdges(RectangleEdge::None);
        setFont(style::status_bar::tethering::font);
        setTextEllipsisType(TextEllipsis::Right);
        setText(utils::translate("statusbar_tethering"));
        setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
    }
} // namespace gui::status_bar
