// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Icon.hpp"
#include "TextParse.hpp"
#include <i18n/i18n.hpp>
#include <Style.hpp>
#include <Font.hpp>

using namespace gui;

Icon::Icon(Item *parent,
           const uint32_t &x,
           const uint32_t &y,
           const uint32_t &w,
           const uint32_t &h,
           const UTF8 &imageName,
           const UTF8 &str)
    : VBox(parent, x, y, w, h)
{
    setEdges(RectangleEdge::None);
    setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

    image = new Image(this, imageName);
    image->setMargins(Margins(0, icon::image_top_margin, 0, icon::image_bottom_margin));

    text = new Text(this, 0, 0, 0, 0);
    text->setMaximumSize(w, h);
    text->setTextType(TextType::MultiLine);
    text->setEditMode(EditMode::Browse);
    text->setEdges(RectangleEdge::None);
    text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    text->setFont(style::window::font::medium);
    text->setRichText(str);
}
