// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <TextFixedSize.hpp>
#include "OptionSetting.hpp"
#include "widgets/ButtonTriState.hpp"
#include <Image.hpp>

namespace gui::option
{
    auto OptionSettings::build() const -> ListItem *
    {
        auto optionItem = new gui::ListItem();
        optionItem->setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        optionItem->setMargins(Margins(0, 0, 0, window::option_bottom_margin));
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
        optionText->setMaximumSize(style::window::default_body_width, style::window::label::big_h);
        optionText->setMargins(Margins(window::option_left_margin, 0, 0, 0));
        optionText->setFont(style::window::font::big);
        optionText->setRichText(text);

        std::string imageName;
        ButtonTriState *button = nullptr;

        switch (rightItem) {
        case SettingRightItem::ArrowBlack:
            imageName = "arrow_right_32px_W_G";
            break;
        case SettingRightItem::ArrowWhite:
            imageName = "arrow_right_empty_32px_W_G";
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
            auto optionTextRight = new TextFixedSize(optionBodyHBox, 0, 0, 0, 0);
            optionTextRight->drawUnderline(false);
            optionTextRight->setFont((textOnRightIsSmall) ? style::window::font::verysmall
                                                          : style::window::font::medium);
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

        if (button) {
            button->setMargins(Margins(0, 0, window::option_right_margin, 0));
        }

        if (indent) {
            optionText->setMargins(Margins(window::option_left_margin * 2, 0, 0, 0));
        }

        return optionItem;
    }
} // namespace gui::option
