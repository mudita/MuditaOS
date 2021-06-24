// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationListItems.hpp"
#include "Style.hpp"

#include <i18n/i18n.hpp>

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

PreparationTimeItem::PreparationTimeItem(std::chrono::seconds _duration)
    : MeditationListItem(convertToPrintable(_duration)), duration(_duration)
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

void PreparationTimeItem::select(bool isSelected)
{
    imageSelectionTick->setVisible(isSelected);
}

std::string PreparationTimeItem::convertToPrintable(std::chrono::seconds _duration)
{
    if (_duration.count() > 60) {
        return std::to_string(static_cast<int>((_duration.count()) / 60)) + " m";
    }
    return std::to_string(static_cast<int>(_duration.count())) + " s";
}

OptionItemMeditationCounter::OptionItemMeditationCounter(bool isCounterOn)
    : MeditationListItem(utils::translate("app_meditation_option_show_counter"))
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

void OptionItemMeditationCounter::select(bool isSelected)
{
    imageOptionOn->setVisible(isSelected);
    imageOptionOff->setVisible(!isSelected);
}

OptionItemPreparation::OptionItemPreparation() : MeditationListItem(utils::translate("app_meditation_preparation_time"))
{
    image = new gui::Image(this,
                           listStyle::image::X,
                           listStyle::image::Y,
                           listStyle::image::Width,
                           listStyle::image::Height,
                           "right_label_arrow_border");
    image->setVisible(true);
}
