#include "MeditationListItems.hpp"
#include "application-meditation/data/Style.hpp"

using namespace gui;
namespace listStyle = style::meditation::itemList;

MeditationListItem::MeditationListItem(std::string textValue)
{
    setMargins(Margins(0, style::margins::big, 0, 0));
    setMinimumSize(listStyle::text::Width, listStyle::text::Height);
    setMaximumSize(listStyle::text::Width, listStyle::text::Height);

    setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
    setPenFocusWidth(style::window::default_border_focus_w);
    setPenWidth(style::window::default_border_no_focus_w);

    text = new gui::Label(
        this, listStyle::text::X, listStyle::text::Y, listStyle::text::Width, listStyle::text::Height, textValue);

    style::window::decorate(text);
    text->setFont(style::window::font::medium);
    text->setEllipsis(Ellipsis::Right);
    text->setVisible(true);
}

PrepTimeItem::PrepTimeItem(std::string text) : MeditationListItem(std::move(text))
{
    MeditationListItem::text->setFont(style::window::font::big);
    imageSelectionTick = new gui::Image(this,
                                        listStyle::image::X,
                                        listStyle::image::Y,
                                        listStyle::image::Width,
                                        listStyle::image::Height,
                                        "small_tick_W_M");
    imageSelectionTick->setVisible(false);
    imageSelectionTick->setAlignment(Alignment(Alignment::Horizontal::Right));
}

void PrepTimeItem::select(bool value)
{
    imageSelectionTick->setVisible(value);
}

OptionItemMeditationCounter::OptionItemMeditationCounter(std::string text, bool isCounterOn)
    : MeditationListItem(std::move(text))
{
    imageOptionOn  = new gui::Image(this,
                                   listStyle::image::X,
                                   listStyle::image::OnOffY,
                                   listStyle::image::Width,
                                   listStyle::image::Height,
                                   "btn_on");
    imageOptionOff = new gui::Image(this,
                                    listStyle::image::X,
                                    listStyle::image::OnOffY,
                                    listStyle::image::Width,
                                    listStyle::image::Height,
                                    "btn_off");

    imageOptionOn->setVisible(isCounterOn);
    imageOptionOff->setVisible(!isCounterOn);
}

void OptionItemMeditationCounter::select(bool value)
{
    imageOptionOn->setVisible(value);
    imageOptionOff->setVisible(!value);
}

OptionItemPreparation::OptionItemPreparation(std::string text) : MeditationListItem(std::move(text))
{
    image = new gui::Image(this,
                           listStyle::image::X,
                           listStyle::image::Y,
                           listStyle::image::Width,
                           listStyle::image::Height,
                           "right_label_arrow_border");
    image->setVisible(true);
}
