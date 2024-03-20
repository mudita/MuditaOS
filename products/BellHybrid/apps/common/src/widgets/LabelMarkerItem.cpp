// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/LabelMarkerItem.hpp>
#include "common/options/OptionBellMenu.hpp"

namespace gui
{
    LabelMarkerItem::LabelMarkerItem(const UTF8 &labelText)
    {
        constexpr auto linesMaxNumber{1U};

        setMinimumSize(style::bell_options::default_text_width, style::bell_options::h);
        setMargins(gui::Margins(0, style::bell_options::option_margin, 0, style::bell_options::option_margin));
        setAlignment(gui::Alignment::Horizontal::Center);
        activatedCallback    = nullptr;
        focusChangedCallback = nullptr;

        auto labelBodyHBox = new gui::HBox(this, 0, 0, 0, 0);
        labelBodyHBox->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        style::window::decorate(labelBodyHBox);

        auto text = new gui::TextFixedSize(labelBodyHBox, 0, 0, 0, 0);
        text->setLines(linesMaxNumber);
        text->setTextType(gui::TextType::SingleLine);
        text->drawUnderline(false);
        text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        text->setMaximumSize(style::bell_options::default_text_width, style::bell_options::h);
        text->setFont(style::window::font::verybigbold);

        text->setRichText(labelText);
        dimensionChangedCallback = [labelBodyHBox](gui::Item &, const gui::BoundingBox &newDim) -> bool {
            labelBodyHBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
        activeItem = false;
    }
} // namespace gui
