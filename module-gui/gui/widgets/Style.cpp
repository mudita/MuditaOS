#include "Style.hpp"

namespace style::window {

void decorate(gui::Label *el)
{
    if (el == nullptr) {
        return;
    }
    el->setPenWidth(label::border_no_focus_w);
    el->setPenFocusWidth(label::border_focucs_w);
    el->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_TOP | gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
}

void decorateOption(gui::Label *el)
{
    if (el == nullptr) {
        return;
    }
    decorate(el);
    el->setMargins(gui::Margins(10, 0, 20, 0));
    el->setFilled(false);
    el->setFont(style::window::font::big);
    el->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
    /// actual design values in px
    el->widgetArea.w = 480 - 3 * 20;
    el->widgetArea.x = 20;
    el->widgetArea.h = 55;
}

};  // namespace style::window::label
