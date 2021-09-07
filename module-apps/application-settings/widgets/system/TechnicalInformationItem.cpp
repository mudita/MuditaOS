// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TechnicalInformationItem.hpp"

#include <application-settings/widgets/SettingsStyle.hpp>

namespace gui
{
    TechnicalInformationItem::TechnicalInformationItem(const std::string &titleText, const std::string &valueText)
    {
        setMinimumSize(style::technical_info::width, style::technical_info::height);
        setMargins(Margins(0, style::margins::big, 0, style::margins::big));
        activeItem = false;

        body = new VBox(this, 0, 0, 0, 0);
        body->setEdges(RectangleEdge::None);

        title = new gui::Text(body, 0, 0, 0, 0);
        title->setMinimumSize(style::technical_info::width, style::technical_info::title_height);
        title->setFont(style::window::font::small);
        title->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        title->setText(titleText);

        value = new gui::Text(body, 0, 0, 0, 0);
        value->setMinimumSize(style::technical_info::width, style::technical_info::value_height);
        value->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        value->setFont(style::window::font::medium);
        value->setText(valueText);

        dimensionChangedCallback = [&]([[maybe_unused]] gui::Item &item, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
} /* namespace gui */
