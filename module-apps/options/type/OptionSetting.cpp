// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <TextFixedSize.hpp>
#include "OptionSetting.hpp"
#include "widgets/ButtonOnOff.hpp"
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
        optionText->setUnderline(false);
        optionText->setAlignment(gui::Alignment::Vertical::Center);
        optionText->setMaximumSize(style::window::default_body_width, style::window::label::big_h);
        optionText->setMargins(Margins(window::option_left_margin, 0, 0, 0));
        optionText->setFont(style::window::font::big);
        optionText->setRichText(text);

        std::string imageName;
        ButtonOnOff *button = nullptr;

        switch (rightItem) {
        case SettingRightItem::ArrowBlack:
            imageName = "right_label_arrow";
            break;
        case SettingRightItem::ArrowWhite:
            imageName = "right_label_arrow_border";
            break;
        case SettingRightItem::On:
            button = new ButtonOnOff(optionBodyHBox, ButtonState::On);
            break;
        case SettingRightItem::Off:
            button = new ButtonOnOff(optionBodyHBox, ButtonState::Off);
            break;
        case SettingRightItem::Bt:
            imageName = "Bluetooth_small";
            break;
        case SettingRightItem::Checked:
            imageName = "small_tick_W_M";
            break;
        case SettingRightItem::Text: {
            auto optionTextRight = new TextFixedSize(optionBodyHBox, 0, 0, 0, 0);
            optionTextRight->setUnderline(false);
            optionTextRight->setMinimumSize(gui::option::window::option_right_min_size, style::window::label::big_h);
            optionTextRight->setAlignment(
                gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
            optionTextRight->setMargins(Margins(0, 0, window::option_right_margin, 0));
            optionTextRight->setFont(style::window::font::medium);
            optionTextRight->setRichText(textOnRight);
            break;
        }
        default:
            break;
        }

        if (!imageName.empty()) {
            auto image = new gui::Image(optionBodyHBox, 0, 0, 0, 0, imageName);
            image->setMargins(Margins(0, 0, window::option_right_margin, 0));
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
