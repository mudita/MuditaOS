// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <TextFixedSize.hpp>
#include "OptionBellMenu.hpp"

namespace gui::option
{
    auto OptionBellMenu::build() const -> ListItem *
    {
        auto optionItem = new gui::ListItem();
        optionItem->setMinimumSize(bell::window::default_text_width, style::window::label::big_h);
        optionItem->setMargins(Margins(0, 0, 0, bell::window::option_bottom_margin));
        optionItem->setAlignment(gui::Alignment::Horizontal::Center);
        optionItem->activatedCallback    = activatedCallback;
        optionItem->focusChangedCallback = focusChangedCallback;

        auto optionBodyHBox = new HBox(optionItem, 0, 0, 0, 0);
        optionBodyHBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        style::window::decorate(optionBodyHBox);

        optionItem->dimensionChangedCallback = [optionBodyHBox](gui::Item &, const BoundingBox &newDim) -> bool {
            optionBodyHBox->setPosition(0, 0);
            optionBodyHBox->setSize(newDim.w, newDim.h);
            return true;
        };

        auto optionText = new TextFixedSize(optionBodyHBox, 0, 0, 0, 0);
        optionText->drawUnderline(false);
        optionText->setAlignment(gui::Alignment::Vertical::Center);
        optionText->setAlignment(gui::Alignment::Horizontal::Center);
        optionText->setMaximumSize(bell::window::default_text_width, style::window::label::big_h);
        optionText->setMargins(Margins(bell::window::option_left_margin, 0, 0, 0));
        optionText->setFont(style::window::font::largelight);
        optionText->setRichText(text);

        return optionItem;
    }
} // namespace gui::option
