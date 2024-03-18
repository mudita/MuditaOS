// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/LabelOptionWithTick.hpp>
#include <common/widgets/LabelListItem.hpp>

namespace gui::option
{
    LabelOptionWithTick::LabelOptionWithTick(ListLabel label,
                                             const UTF8 &text,
                                             TickState tickState,
                                             std::function<bool(Item &)> activatedCallback,
                                             std::function<bool(Item &)> focusChangedCallback,
                                             gui::AppWindow *app)
        : OptionBellMenu(text, std::move(activatedCallback), std::move(focusChangedCallback), app),
          label{std::move(label)}, tickState{tickState}
    {}

    auto LabelOptionWithTick::build() const -> ListItem *
    {
        auto labelOption = new LabelListItem(label);
        prepareLabelOption(labelOption);
        return labelOption;
    }

    auto LabelOptionWithTick::prepareLabelOption(ListItem *labelOption) const -> void
    {
        constexpr auto linesMaxNumber{1U};
        constexpr auto tickMarginRight{12U};

        labelOption->setMinimumSize(style::bell_options::default_text_width, style::bell_options::h);
        labelOption->setMargins(Margins(0, style::bell_options::option_margin, 0, style::bell_options::option_margin));
        labelOption->setAlignment(gui::Alignment::Horizontal::Center);
        labelOption->activatedCallback    = activatedCallback;
        labelOption->focusChangedCallback = focusChangedCallback;

        auto labelOptionBody = new HBox(labelOption, 0, 0, 0, 0);
        labelOptionBody->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        style::window::decorate(labelOptionBody);

        Image *tickImage      = nullptr;
        auto optionTextMargin = 0;
        auto optionTextWidth  = style::bell_options::default_text_width;
        if (tickState == TickState::Show) {
            tickImage                  = new Image(nullptr, 0, 0, 0, 0, "bell_small_tick_W_M");
            const auto imageTotalSpace = static_cast<int>(tickImage->getWidth() + tickMarginRight);
            optionTextMargin           = imageTotalSpace;
            optionTextWidth -= imageTotalSpace * 2; // Option text has to be centered, so clip space from both sides
        }
        auto labelOptionText = new TextFixedSize(labelOptionBody, 0, 0, 0, 0);
        labelOptionText->setLines(linesMaxNumber);
        labelOptionText->setTextType(gui::TextType::SingleLine);
        labelOptionText->drawUnderline(false);
        labelOptionText->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        labelOptionText->setMargins(Margins{optionTextMargin, 0, 0, 0});
        labelOptionText->setMaximumSize(optionTextWidth, style::bell_options::h);
        labelOptionText->setFont(style::window::font::largelight);
        labelOptionText->setRichText(getAdjustedText(labelOptionText));

        /* Add tick after option text to resolve weird alignment issues */
        if (tickImage != nullptr) {
            tickImage->setAlignment(Alignment{Alignment::Horizontal::Right, Alignment::Vertical::Center});
            tickImage->setMargins(Margins{0, 0, tickMarginRight, 0});
            tickImage->setVisible(true);
            labelOptionBody->addWidget(tickImage);
        }

        labelOption->dimensionChangedCallback = [labelOptionBody](gui::Item &, const BoundingBox &newDim) -> bool {
            labelOptionBody->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    auto LabelOptionWithTick::getAdjustedText(TextFixedSize *textItem) const -> UTF8
    {
        constexpr auto ellipsis{"..."};
        constexpr auto ellipsisSpace{2U};

        const auto maxCharsInLine =
            textItem->getTextFormat().getFont()->getCharCountInSpace(text, textItem->widgetMaximumArea.w);
        if (maxCharsInLine < text.length()) {
            return text.substr(0, maxCharsInLine - ellipsisSpace) + ellipsis;
        }
        return text;
    }
} // namespace gui::option
