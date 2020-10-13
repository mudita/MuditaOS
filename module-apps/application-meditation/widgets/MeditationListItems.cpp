#include "MeditationListItems.hpp"

using namespace gui;
namespace currentStyle = style::meditation::item;

MeditationListItem::MeditationListItem(std::string textValue)
{
    setMargins(Margins(0, style::margins::big, 0, 0));
    setMinimumSize(currentStyle::w, currentStyle::text::h);
    setMaximumSize(currentStyle::w, currentStyle::text::h);

    setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    setPenFocusWidth(style::window::default_border_focus_w);
    setPenWidth(style::window::default_border_no_focus_w);

    text = new gui::Label(this, 0, 0, currentStyle::text::w, currentStyle::text::h, textValue);
    style::window::decorate(text);
    text->setFont(style::window::font::medium);
    text->setEllipsis(Ellipsis::Right);
    text->setVisible(true);
}

PrepTimeItem::PrepTimeItem(std::string text) : MeditationListItem(std::move(text))
{
    MeditationListItem::text->setFont(style::window::font::big);
    imageSelectionTick = new gui::Image(this, currentStyle::img::x, currentStyle::img::y, 0, 0, "small_tick_W_M");
    imageSelectionTick->setVisible(false);
    imageSelectionTick->setAlignment(Alignment(Alignment::Horizontal::Right));
}

void PrepTimeItem::select(bool value)
{
    imageSelectionTick->setVisible(value);
}

OptionItem1::OptionItem1(std::string text) : MeditationListItem(std::move(text))
{
    imageOptionOn  = new gui::Image(this, currentStyle::img::x, currentStyle::img::y, 0, 0, "small_tick_W_M");
    imageOptionOff = new gui::Image(this, currentStyle::img::x, currentStyle::img::y, 0, 0, "small_tick_W_M");

    imageOptionOn->setVisible(true);
    imageOptionOff->setVisible(false);
}

void OptionItem1::select(bool value)
{
    imageOptionOn->setVisible(value);
    imageOptionOff->setVisible(!value);
}

OptionItem2::OptionItem2(std::string text) : MeditationListItem(std::move(text))
{
    image = new gui::Image(this, currentStyle::img::x, currentStyle::img::y, 0, 0, "small_tick_W_M");
    image->setVisible(true);
}
