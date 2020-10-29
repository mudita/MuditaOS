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

        auto optionNameLabel = new Label(optionBodyHBox, 0, 0, 0, 0, text);
        optionNameLabel->setMinimumSize(style::window::default_body_width - style::buttonOnOff::w,
                                        style::window::label::big_h);
        optionNameLabel->setEdges(RectangleEdge::None);
        optionNameLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        optionNameLabel->setFont(style::window::font::big);

        std::string imageName;
        switch (rightItem) {
        case RightItem::ArrowBlack:
            imageName = "right_label_arrow";
            break;
        case RightItem::ArrowWhite:
            imageName = "right_label_arrow_border";
            break;
        case RightItem::On:
            new ButtonOnOff(optionBodyHBox, ButtonState::On);
            break;
        case RightItem::Off:
            new ButtonOnOff(optionBodyHBox, ButtonState::Off);
            break;
        case RightItem::Bt:
            imageName = "bt";
            break;
        default:
            break;
        }

        if (!imageName.empty()) {
            auto image = new gui::Image(optionBodyHBox, 0, 0, 0, 0, imageName);
            optionNameLabel->setMinimumSize(style::window::default_body_width - image->getWidth(),
                                            style::window::label::big_h);
        }

        return optionBodyHBox;
    }
} // namespace gui
