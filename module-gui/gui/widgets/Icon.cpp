#include "Icon.hpp"
#include "TextParse.hpp"
#include <i18/i18.hpp>
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
        constexpr uint32_t x  = 40;
        constexpr uint32_t y  = 247;
        constexpr uint32_t w  = 400;
        constexpr uint32_t h  = 66;
        constexpr Color color = {7, 0};
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
    setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    setPenFocusWidth(style::window::default_border_no_focus_w);
    setPenWidth(style::window::default_border_no_focus_w);

    new Image(this, style::img::x, style::img::y, imageName);
    text = new Text(this, style::text::x, style::text::y, style::text::w, style::text::h);
    text->setTextType(TextType::MULTI_LINE);
    text->setEditMode(EditMode::BROWSE);
    text->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

    auto format = TextFormat(Font(27).raw(), Color(7, 0));
    for (auto &el : textToTextBlocks(str, format)) {
        text->addText(el);
    }
}
