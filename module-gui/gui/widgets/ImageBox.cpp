#include "ImageBox.hpp"

using namespace gui;

ImageBox::ImageBox(
    Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, Image *image)
    : HBox(parent, x, y, w, h)
{
    setEdges(RectangleEdge::None);
    setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    setPenFocusWidth(style::window::default_border_focus_w);
    setPenWidth(style::window::default_border_no_focus_w);
    addWidget(image);

    focusChangedCallback = [&](Item &item) {
        if (focus) {
            setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
        }
        else {
            setEdges(RectangleEdge::None);
        }
        return true;
    };
}
