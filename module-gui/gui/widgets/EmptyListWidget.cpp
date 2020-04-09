#include "EmptyListWidget.hpp"
#include <i18/i18.hpp>
#include <Style.hpp>

using namespace gui;

EmptyListWidget::EmptyListWidget(Item *parent,
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

    icon = new Image(this, 176, 90, 128, 128, imageName);

    text = new Text(this, 40, 247, 400, 66);
    text->setText(str);
    text->setTextType(Text::TextType::MULTI_LINE);
    text->setEditMode(Text::EditMode::BROWSE);
    text->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    text->setFont(style::window::font::medium);
    text->setAlignment(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
    text->setTextColor(Color(11, 0));
}
