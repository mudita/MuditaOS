// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionSetting.hpp"
#include "widgets/ButtonOnOff.hpp"

namespace gui
{
    auto OptionSettings::build() const -> Item *
    {
        auto optionBodyHBox = new HBox(nullptr, 0, 0, style::window::default_body_width, style::window::label::big_h);
        optionBodyHBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        optionBodyHBox->activatedCallback    = activatedCallback;
        optionBodyHBox->focusChangedCallback = focusCb;
        style::window::decorate(optionBodyHBox);

        auto optionNameLabel =
            new Label(optionBodyHBox, indent ? style::window::default_left_margin : 0, 0, 0, 0, text);
        optionNameLabel->setEdges(RectangleEdge::None);
        optionNameLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        optionNameLabel->setFont(style::window::font::big);

        std::string imageName;
        int optionRightItemWidth = 0;

        switch (rightItem) {
        case RightItem::ArrowBlack:
            imageName = "right_label_arrow";
            break;
        case RightItem::ArrowWhite:
            imageName = "right_label_arrow_border";
            break;
        case RightItem::On:
            new ButtonOnOff(optionBodyHBox, ButtonState::On);
            optionRightItemWidth = style::buttonOnOff::w;
            break;
        case RightItem::Off:
            new ButtonOnOff(optionBodyHBox, ButtonState::Off);
            optionRightItemWidth = style::buttonOnOff::w;
            break;
        case RightItem::Bt:
            imageName = "bt";
            break;
        case RightItem::Checked:
            imageName = "small_tick_W_M";
            break;
        default:
            break;
        }

        if (!imageName.empty()) {
            auto image           = new gui::Image(optionBodyHBox, 0, 0, 0, 0, imageName);
            optionRightItemWidth = image->getWidth();
        }
        if (indent) {
            optionRightItemWidth += style::window::default_left_margin;
        }

        optionNameLabel->setMinimumSize(style::window::default_body_width - optionRightItemWidth,
                                        style::window::label::big_h);
        return optionBodyHBox;
    }
} // namespace gui
