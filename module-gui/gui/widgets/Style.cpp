#include "Style.hpp"
#include <Label.hpp>

namespace style::window
{

    void decorate(gui::Label *el)
    {
        if (el == nullptr) {
            return;
        }
        el->setPenWidth(default_border_no_focus_w);
        el->setPenFocusWidth(default_border_focucs_w);
        el->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_TOP | gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        el->setAlignment(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
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
        /// actual design values in px
        el->widgetArea.w = style::window_width - 3 * 20;
        el->widgetArea.x = 20;
        el->widgetArea.h = style::window::label::big_h;
    }

}; // namespace style::window
