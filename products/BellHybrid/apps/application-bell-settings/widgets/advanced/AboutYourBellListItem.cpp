// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AboutYourBellListItem.hpp"

#include <BellSettingsStyle.hpp>
#include <Text.hpp>

namespace gui
{
    AboutYourBellListItem::AboutYourBellListItem(const std::string &titleText,
                                                 const std::string &valueText,
                                                 TokenMap valueTokenMap)
    {
        setMinimumSize(bell_settings_style::about_your_bell_window::list_item::width,
                       bell_settings_style::about_your_bell_window::list_item::height);
        setMargins(Margins(0, style::margins::big, 0, style::margins::huge));
        activeItem = false;

        body = new VBox(this, 0, 0, 0, 0);
        body->setEdges(RectangleEdge::None);

        title = new Text(body, 0, 0, 0, 0);
        title->setMinimumSize(bell_settings_style::about_your_bell_window::list_item::width,
                              bell_settings_style::about_your_bell_window::list_item::title_height);
        title->setFont(style::window::font::bigbold);
        title->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        title->setText(titleText);

        value = new Text(body, 0, 0, 0, 0);
        value->setMinimumSize(bell_settings_style::about_your_bell_window::list_item::width,
                              bell_settings_style::about_your_bell_window::list_item::value_height);
        value->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        value->setFont(style::window::font::big);
        if (valueTokenMap.has_value()) {
            value->setRichText(valueText, std::move(*valueTokenMap));
        }
        else {
            value->setText(valueText);
        }

        dimensionChangedCallback = [&]([[maybe_unused]] Item &item, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
} /* namespace gui */
