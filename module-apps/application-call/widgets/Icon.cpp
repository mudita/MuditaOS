#include "Icon.hpp"
#include "../data/CallAppStyle.hpp"
#include <log/log.hpp>

using namespace callAppStyle;
namespace gui
{

    Icon::Icon(
        Item *parent, const uint32_t &x, const uint32_t &y, const std::string &imageName, const std::string &labelStr)
        : Rect(parent, x, y, icon::w, icon::h)
    {
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
        setPenFocusWidth(style::window::default_border_focucs_w);
        setPenWidth(style::window::default_border_no_focus_w);

        img   = new gui::Image(this, icon::img::x, icon::img::y, icon::img::w, icon::img::h, imageName);
        label = new gui::Label(this, icon::label::x, icon::label::y, icon::label::w, icon::label::h, labelStr);
        label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        label->setAlignement(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
        label->setFont(style::window::font::verysmall);
    }

    void Icon::setImg(const std::string &imageName)
    {
        img->set(imageName);
    }
} // namespace gui