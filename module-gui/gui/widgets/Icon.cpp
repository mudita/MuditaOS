// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Icon.hpp"
#include "TextParse.hpp"
#include <i18n/i18n.hpp>
#include <Style.hpp>
#include <Font.hpp>

using namespace gui;

namespace style
{
    namespace img
    {
        constexpr uint32_t x = 176;
        constexpr uint32_t y = 90;
    } // namespace img
    namespace text
    {
        constexpr uint32_t x = 40;
        constexpr uint32_t y = 247;
        constexpr uint32_t w = 400;
        constexpr uint32_t h = 80;
    } // namespace text
} // namespace style

Icon::Icon(Item *parent,
           const uint32_t &x,
           const uint32_t &y,
           const uint32_t &w,
           const uint32_t &h,
           const UTF8 imageName,
           const UTF8 &str)
    : Rect(parent, x, y, w, h)
{
    setEdges(RectangleEdge::None);
    setPenFocusWidth(style::window::default_border_no_focus_w);
    setPenWidth(style::window::default_border_no_focus_w);

    new Image(this, style::img::x, style::img::y, imageName);
    text = new Text(this, style::text::x, style::text::y, style::text::w, style::text::h);
    text->setRichText(str);
    text->setTextType(TextType::MultiLine);
    text->setEditMode(EditMode::Browse);
    text->setEdges(RectangleEdge::None);
    text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
}
