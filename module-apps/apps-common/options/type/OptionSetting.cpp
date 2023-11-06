// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <TextFixedSize.hpp>
#include "OptionSetting.hpp"
#include "widgets/ButtonTriState.hpp"
#include <Image.hpp>

namespace gui::option
{
    OptionSettings::OptionSettings(const UTF8 &primaryText,
                                   std::function<bool(Item &)> activatedCallback,
                                   std::function<bool(Item &)> focusChangedCallback,
                                   gui::AppWindow *app,
                                   gui::option::SettingRightItem rightItem,
                                   bool indent,
                                   const UTF8 &textOnRight,
                                   bool isTextOnRightSmall,
                                   const UTF8 &secondaryText)
        : primaryText(primaryText), secondaryText(secondaryText), activatedCallback(std::move(activatedCallback)),
          focusChangedCallback(std::move(focusChangedCallback)), app(app), rightItem(rightItem), indent(indent),
          textOnRight(textOnRight), isTextOnRightSmall(isTextOnRightSmall)
    {}

    auto OptionSettings::build() const -> ListItem *
    {
        const auto optionItem = new gui::ListItem();
        optionItem->setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        optionItem->setMargins(Margins(0, 0, 0, window::option_bottom_margin));
        optionItem->activatedCallback    = activatedCallback;
        optionItem->focusChangedCallback = focusChangedCallback;

        const auto optionBodyHBox = new HBox(optionItem, 0, 0, 0, 0);
        optionBodyHBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        style::window::decorate(optionBodyHBox);

        optionItem->dimensionChangedCallback = [optionBodyHBox](gui::Item &, const BoundingBox &newDim) -> bool {
            optionBodyHBox->setPosition(0, 0);
            optionBodyHBox->setSize(newDim.w, newDim.h);
            return true;
        };

        const auto primaryOptionText = new TextFixedSize(optionBodyHBox, 0, 0, 0, 0);
        primaryOptionText->drawUnderline(false);
        primaryOptionText->setAlignment(gui::Alignment::Vertical::Center);
        primaryOptionText->setMargins(Margins(window::option_left_margin, 0, 0, 0));
        primaryOptionText->setFont(style::window::font::big);
        primaryOptionText->setRichText(primaryText);

        if (!secondaryText.empty()) {
            /* Set primary text box size to use only the space required for text to be shown */
            primaryOptionText->setMinimumHeightToFitText();
            primaryOptionText->setMinimumWidthToFitText();

            /* Set secondary text box to take all the remaining space */
            const auto secondaryOptionText = new TextFixedSize(optionBodyHBox, 0, 0, 0, 0);
            secondaryOptionText->drawUnderline(false);
            secondaryOptionText->setAlignment(
                Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
            secondaryOptionText->setMaximumSize(style::window::default_body_width, style::window::label::small_h);
            secondaryOptionText->setMargins(Margins(window::option_label_spacer, 0, 0, 0));
            secondaryOptionText->setFont(style::window::font::small);
            secondaryOptionText->setRichText(secondaryText);
        }
        else {
            /* Set primary text box to take all the space */
            primaryOptionText->setMaximumSize(style::window::default_body_width, style::window::label::big_h);
        }

        std::string imageName;
        ButtonTriState *button = nullptr;

        switch (rightItem) {
        case SettingRightItem::ArrowBlack:
            imageName = "arrow_right_32px_W_G";
            break;
        case SettingRightItem::ArrowWhite:
            imageName = "arrow_right_empty_32px_W_G";
            break;
        case SettingRightItem::Transiting:
            button = new ButtonTriState(optionBodyHBox, ButtonTriState::State::Transiting);
            break;
        case SettingRightItem::On:
            button = new ButtonTriState(optionBodyHBox, ButtonTriState::State::On);
            break;
        case SettingRightItem::Off:
            button = new ButtonTriState(optionBodyHBox, ButtonTriState::State::Off);
            break;
        case SettingRightItem::Bt:
            imageName = "bluetooth_32px_W_M";
            break;
        case SettingRightItem::Checked:
            imageName = "small_tick_32px_W_M";
            break;
        case SettingRightItem::SIM1:
            imageName = "sim1_option_32px_W_G";
            break;
        case SettingRightItem::SIM2:
            imageName = "sim2_option_32px_W_G";
            break;
        case SettingRightItem::Text: {
            const auto optionTextRight = new TextFixedSize(optionBodyHBox, 0, 0, 0, 0);
            optionTextRight->drawUnderline(false);
            optionTextRight->setFont(isTextOnRightSmall ? style::window::font::verysmall : style::window::font::medium);
            optionTextRight->setMinimumWidthToFitText(textOnRight);
            optionTextRight->setMinimumHeight(style::window::label::big_h);
            optionTextRight->setAlignment(
                gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
            optionTextRight->setMargins(Margins(0, 0, window::option_right_margin, 0));
            optionTextRight->setRichText(textOnRight);
            break;
        }
        default:
            break;
        }

        if (!imageName.empty()) {
            new gui::Image(optionBodyHBox, 0, 0, 0, 0, imageName);
        }

        if (button != nullptr) {
            button->setMargins(Margins(0, 0, window::option_right_margin, 0));
        }

        if (indent) {
            primaryOptionText->setMargins(Margins(window::option_left_margin * 2, 0, 0, 0));
        }

        return optionItem;
    }

    auto OptionSettings::str() const -> std::string
    {
        return primaryText + secondaryText;
    }
} // namespace gui::option
