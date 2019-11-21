#include "Icon.hpp"
#include <Style.hpp>
#include <log/log.hpp>

namespace gui
{
namespace icon
{
constexpr uint32_t w = 100;
constexpr uint32_t h = 100;     
namespace label
{
constexpr uint32_t x = 9;
constexpr uint32_t y = 58;
constexpr uint32_t w = 82;
constexpr uint32_t h = 20;
} // namespace label
namespace img
{
constexpr uint32_t x = 34;
constexpr uint32_t y = 15;
constexpr uint32_t w = 32;
constexpr uint32_t h = 32;
} // namespace img
} // namespace icon

Icon::Icon(Item *parent, const uint32_t &x, const uint32_t &y, const std::string &imgStr, const std::string &labelStr)
    : Rect(parent, x, y, icon::w, icon::h)
{
    setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP );
	setPenFocusWidth(2);
	setPenWidth(0);

    img = new gui::Image(this, icon::img::x, icon::img::y, icon::img::w, icon::img::h, imgStr);
    label = new gui::Label(this, icon::label::x, icon::label::y, icon::label::w, icon::label::h, labelStr);
    style::window::decorate(label);
    label->setFont(style::window::font::verysmall);
}

} // namespace gui