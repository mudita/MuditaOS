// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <options/OptionBellMenu.hpp>
#include <TextFixedSize.hpp>

namespace
{
    constexpr auto ellipsis       = "...";
    constexpr auto ellipsisSpace  = 2u;
    constexpr auto linesMaxNumber = 1u;

    UTF8 adjustTextLength(const UTF8 &text, const std::uint32_t maxCharsInLine)
    {
        if (maxCharsInLine < text.length()) {
            return text.substr(0, maxCharsInLine - ellipsisSpace) + ellipsis;
        }
        return text;
    }
} // namespace

namespace gui::option
{
    auto OptionBellMenu::build() const -> ListItem *
    {
        auto optionItem = new gui::ListItem();
        optionItem->setMinimumSize(style::bell_options::default_text_width, style::bell_options::h);
        optionItem->setMargins(Margins(0, style::bell_options::option_margin, 0, style::bell_options::option_margin));
        optionItem->setAlignment(gui::Alignment::Horizontal::Center);
        optionItem->activatedCallback    = activatedCallback;
        optionItem->focusChangedCallback = focusChangedCallback;

        auto optionBodyHBox = new HBox(optionItem, 0, 0, 0, 0);
        optionBodyHBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        style::window::decorate(optionBodyHBox);

        auto optionText = new TextFixedSize(optionBodyHBox, 0, 0, 0, 0);
        optionText->setLines(linesMaxNumber);
        optionText->setTextType(gui::TextType::SingleLine);
        optionText->drawUnderline(false);
        optionText->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        optionText->setMaximumSize(style::bell_options::default_text_width, style::bell_options::h);
        optionText->setFont(style::window::font::largelight);
        const auto maxTextCharsToDisplay =
            optionText->getTextFormat().getFont()->getCharCountInSpace(text, style::bell_options::default_text_width);
        optionText->setRichText(adjustTextLength(text, maxTextCharsToDisplay));
        optionItem->dimensionChangedCallback = [optionBodyHBox](gui::Item &, const BoundingBox &newDim) -> bool {
            optionBodyHBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        return optionItem;
    }
} // namespace gui::option
