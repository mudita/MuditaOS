// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionSimple.hpp"
#include "OptionStyle.hpp"
#include "FontManager.hpp"
#include "Text.hpp"
#include "TextFixedSize.hpp"
#include <Image.hpp>

namespace gui::option
{
    Simple::Simple(UTF8 text, std::function<bool(Item &)> activatedCallback, Arrow arrow)
        : text(std::move(text)), activatedCallback(std::move(activatedCallback)), arrow(arrow)
    {}

    auto Simple::build() const -> ListItem *
    {
        auto optionItem = new gui::ListItem();
        optionItem->setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        optionItem->setMargins(Margins(0, 0, 0, window::option_bottom_margin));

        auto optionBodyHBox = new gui::HBox(optionItem, 0, 0, 0, 0);
        optionBodyHBox->setEdges(RectangleEdge::None);
        optionBodyHBox->setAlignment(gui::Alignment::Vertical::Center);

        auto optionText = new TextFixedSize(optionBodyHBox, 0, 0, 0, 0);
        optionText->setMargins(Margins(window::option_left_margin, 0, 0, 0));
        optionText->drawUnderline(false);
        optionText->setAlignment(gui::Alignment::Vertical::Center);
        optionText->setMaximumSize(style::window::default_body_width, style::window::label::big_h);
        optionText->setFont(style::window::font::big);
        optionText->setRichText(text);

        optionItem->dimensionChangedCallback = [optionBodyHBox](gui::Item &, const BoundingBox &newDim) -> bool {
            optionBodyHBox->setPosition(0, 0);
            optionBodyHBox->setSize(newDim.w, newDim.h);
            return true;
        };

        optionItem->activatedCallback = activatedCallback;

        if (arrow == Arrow::Enabled) {
            new gui::Image(optionBodyHBox, 0, 0, 0, 0, "arrow_right_empty_32px_W_G");
        }

        return optionItem;
    }

} // namespace gui::option
